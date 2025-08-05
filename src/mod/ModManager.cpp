#include "mod/ModManager.hpp"

#include "Debug/DebugManager.hpp"
#include "debug/DebugConsole.hpp"
#include "debug/DebugUtils.hpp"
#include "mod/ModRequirement.hpp"
#include "mod/ScriptEngine.hpp"
#include "mod/ScriptLoader.hpp"
#include "mod/ScriptProvider.hpp"
#include "mod/UnpackagedMod.hpp"
#include "program/Context.hpp"
#include "program/Engine.hpp"
#include "util/Definitions.hpp"
#include "util/EnumFlag.hpp"
#include "util/Version.hpp"

#include <memory>
#include <optional>
#include <vector>

using namespace tudov;

ModManager::ModManager(Context &context) noexcept
    : _log(Log::Get("ModManager")),
      _context(context),
      _loadState(ELoadState::None)
{
	_virtualUnpackagedMod = std::make_unique<UnpackagedMod>(*this, "");
}

ModManager::~ModManager() noexcept
{
	UnloadMods();
}

Context &ModManager::GetContext() noexcept
{
	return _context;
}

void ModManager::Initialize() noexcept
{
	// _directories = {
	//     "mods",
	//     "downloadedMods",
	// };
	_modDirectories = {
	    {EGlobalStorageLocation::Application, "mods"},
	    {EGlobalStorageLocation::User, "mods"},
	};
	_requiredMods = {
	    ModRequirement("dr2c_e0c8375e09d74bb9aa704d4a3c4afa79", Version(1, 0, 0), 0),
	};
	_hotReloadScriptsPending = nullptr;
}

void ModManager::Deinitialize() noexcept
{
	// eventManager.DetachFromScriptLoader(scriptEngine.scriptLoader);
	// eventManager.UninstallFromScriptEngine(scriptEngine);
}

bool ModManager::IsNoModMatch() const
{
	return _requiredMods.empty();
}

bool ModManager::IsModMatched(const Mod &mod) const
{
	auto &&uid = mod.IMod::GetConfig().uniqueID;

	for (auto &&modEntry : _requiredMods)
	{
		if (uid == modEntry.uid)
		{
			return true;
		}
	}
	return false;
}

void ModManager::LoadMods()
{
	if (EnumFlag::HasAll(_loadState, ELoadState::Loading))
	{
		return;
	}
	EnumFlag::Mask(_loadState, ELoadState::Loading);

	_log->Debug("Loading all required mods ...");

	if (!_requiredMods.empty())
	{
		UnloadMods();
	}

	if (IsNoModMatch())
	{
		_log->Debug("Loaded all required mods: no required mod specified");
		return;
	}

	std::error_code errorCode;
	std::filesystem::recursive_directory_iterator it{"mods", std::filesystem::directory_options::skip_permission_denied, errorCode}, end;

 	while (it != end)
	{
		const std::filesystem::directory_entry &entry = *it;

		// HANDLEING ZIP FILES (PACKAGED MODS)
		// if (entry.is_regular_file() && entry.path().extension() == ".zip")
		// {
		// }

		if (entry.is_directory())
		{
			auto &&dir = entry.path();
			if (_virtualUnpackagedMod->IsValidDirectory(dir))
			{
				auto &&mod = std::make_shared<UnpackagedMod>(*this, dir);
				if (IsModMatched(*mod))
				{
					_loadedMods.emplace_back(mod);
				}
				it.disable_recursion_pending();
			}
		}

		++it;
	}

	for (auto &&mod : _loadedMods)
	{
		mod->Load();
	}

	GetScriptLoader().LoadAllScripts();

	_log->Debug("Loaded all required mods");

	EnumFlag::Unmask(_loadState, ELoadState::Loading);
}

void ModManager::LoadModsDeferred()
{
	EnumFlag::Mask(_loadState, ELoadState::LoadPending);
}

void ModManager::UnloadMods()
{
	if (EnumFlag::HasAny(_loadState, ELoadState::Unloading))
	{
		return;
	}
	EnumFlag::Mask(_loadState, ELoadState::Unloading);

	_log->Debug("Unloading all loaded mods ...");

	GetScriptLoader().UnloadAllScripts();

	for (auto &&mod : _loadedMods)
	{
		mod->Unload();
	}
	_loadedMods.clear();

	_log->Debug("Unloaded all loaded mods");

	EnumFlag::Unmask(_loadState, ELoadState::Unloading);
}

std::vector<ModListedEntry> ModManager::ListAvailableMods() noexcept
{
	std::vector<ModListedEntry> entries;
	//
	return entries;
}

std::weak_ptr<Mod> ModManager::FindLoadedMod(std::string_view uid) noexcept
{
	for (auto &&mod : _loadedMods)
	{
		if (mod->GetConfig().uniqueID == uid)
		{
			return mod;
		}
	}
	return std::weak_ptr<Mod>();
}

std::vector<ModRequirement> &ModManager::GetRequiredMods() noexcept
{
	return _requiredMods;
}

const std::vector<ModRequirement> &ModManager::GetRequiredMods() const noexcept
{
	return _requiredMods;
}

void ModManager::HotReloadScriptPending(std::string_view scriptName, const std::shared_ptr<TextResource> &scriptCode, std::string_view scriptModUID)
{
	if (!_hotReloadScriptsPending)
	{
		_hotReloadScriptsPending = std::make_unique<HotReloadScriptsMap>();
	}
	_hotReloadScriptsPending->try_emplace(std::string(scriptName), scriptCode, std::string(scriptModUID));
}

void ModManager::Update()
{
	if (EnumFlag::HasAny(_loadState, ELoadState::LoadPending)) [[unlikely]]
	{
		LoadMods();
		EnumFlag::Unmask(_loadState, ELoadState::LoadPending);
		_hotReloadScriptsPending = nullptr;
	}
	else if (_hotReloadScriptsPending) [[unlikely]]
	{
		std::vector<ScriptID> scriptIDs{};

		auto &&scriptLoader = GetScriptLoader();
		auto &&scriptProvider = GetScriptProvider();

		for (auto &&[scriptName, entry] : *_hotReloadScriptsPending)
		{
			auto &&[scriptCode, scriptModUID] = entry;

			auto scriptID = scriptProvider.GetScriptIDByName(scriptName);
			if (scriptID)
			{
				auto &&pendingScripts = GetScriptLoader().UnloadScript(scriptID);
				scriptIDs.insert(scriptIDs.end(), pendingScripts.begin(), pendingScripts.end());

				if (!scriptProvider.RemoveScript(scriptID)) [[unlikely]]
				{
					_log->Warn("Attempt to remove non-exist script id", scriptID);
				}
			}

			scriptID = scriptProvider.AddScript(scriptName, scriptCode, scriptModUID);
			scriptIDs.emplace_back(scriptID);
		}

		std::sort(scriptIDs.begin(), scriptIDs.end());
		scriptIDs.erase(std::unique(scriptIDs.begin(), scriptIDs.end()), scriptIDs.end());

		scriptLoader.HotReloadScripts(scriptIDs);
		GetScriptEngine().CollectGarbage();

		_hotReloadScriptsPending = nullptr;
	}
}

std::vector<DebugConsoleResult> ModManager::DebugAdd(std::string_view arg)
{
	std::vector<DebugConsoleResult> results;

	auto &&[modUID, version] = DebugUtils::Split<2>(arg);

	if (modUID.empty())
	{
		results.emplace_back("Missing ModUID");
		return results;
	}

	std::optional<Version> modVersion = version.empty() ? std::nullopt : std::make_optional<Version>(version);

	return results;
}

void ModManager::ProvideDebug(IDebugManager &debugManager) noexcept
{
	auto &&debugConsole = debugManager.GetElement<DebugConsole>();

	auto &&modAdd = [this](std::string_view arg)
	{
		return DebugAdd(arg);
	};

	debugConsole->SetCommand(DebugConsole::Command{
	    .name = "modAdd",
	    .help = "modAdd <ModUID> [Version]",
	    .func = modAdd,
	});
}
