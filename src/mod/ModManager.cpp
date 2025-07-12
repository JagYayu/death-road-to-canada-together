#include "ModManager.hpp"

#include "ScriptEngine.hpp"
#include "ScriptProvider.hpp"
#include "UnpackagedMod.hpp"
#include "mod/ModEntry.hpp"
#include "program/Context.hpp"
#include "program/Engine.hpp"
#include "util/Defs.hpp"

#include <memory>
#include <thread>
#include <vector>

using namespace tudov;

ModManager::ModManager(Context &context) noexcept
    : _log(Log::Get("ModManager")),
      _context(context),
      _loadState(ELoadState::None),
      _virtualUnpackagedMod(*this, "")
{
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
	_directories = {
	    "mods",
	    "downloadedMods",
	};
	_requiredMods = {
	    ModEntry("dr2c", Version(1, 0, 0), 0),
	};
	_hotReloadScriptsPending = nullptr;
}

void ModManager::Deinitialize() noexcept
{
	// eventManager.DetachFromScriptLoader(scriptEngine.scriptLoader);
	// eventManager.UninstallFromScriptEngine(scriptEngine);
}

void ModManager::AddMod(const std::filesystem::path &modRoot)
{
}

bool ModManager::IsNoModMatch() const
{
	return _requiredMods.empty();
}

bool ModManager::IsModMatched(const Mod &mod) const
{
	auto &&namespace_ = mod.GetNamespace();

	for (auto &&modEntry : _requiredMods)
	{
		if (namespace_ == modEntry.namespace_)
		{
			return true;
		}
	}
	return false;
}

void ModManager::LoadMods()
{
	if ((_loadState & ELoadState::AllLoading) != ELoadState::None)
	{
		return;
	}
	_loadState |= ELoadState::AllLoading;

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
	std::filesystem::recursive_directory_iterator it("mods", std::filesystem::directory_options::skip_permission_denied, errorCode), end;

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
			if (_virtualUnpackagedMod.IsValidDirectory(dir))
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

	GetScriptLoader()->LoadAll();
	GetScriptEngine()->CollectGarbage();

	_log->Debug("Loaded all required mods");

	_loadState &= ~ELoadState::AllLoading;
}

void ModManager::UnloadMods()
{
	if ((_loadState & ELoadState::AllUnloading) != ELoadState::None)
	{
		return;
	}
	_loadState |= ELoadState::AllUnloading;

	_log->Debug("Unloading all loaded mods ...");

	GetScriptLoader()->UnloadAll();

	for (auto &&mod : _loadedMods)
	{
		mod->Unload();
	}
	_loadedMods.clear();

	_log->Debug("Unloaded all loaded mods");

	_loadState &= ~ELoadState::AllUnloading;
}

std::weak_ptr<Mod> ModManager::GetLoadedMod(std::string_view namespace_) noexcept
{
	for (auto &&mod : _loadedMods)
	{
		if (mod->GetNamespace() == namespace_)
		{
			return mod;
		}
	}
	return std::weak_ptr<Mod>();
}

std::vector<ModEntry> &ModManager::GetRequiredMods() noexcept
{
	return _requiredMods;
}

const std::vector<ModEntry> &ModManager::GetRequiredMods() const noexcept
{
	return _requiredMods;
}

void ModManager::HotReloadScriptPending(std::string_view scriptName, std::string_view scriptCode, std::string_view scriptNamespace)
{
	if (!_hotReloadScriptsPending)
	{
		_hotReloadScriptsPending = std::make_unique<THotReloadScriptsMap>();
	}
	_hotReloadScriptsPending->try_emplace(std::string(scriptName), std::string(scriptCode), std::string(scriptNamespace));
}

void ModManager::Update()
{
	if (!_hotReloadScriptsPending)
	{
		return;
	}

	std::vector<ScriptID> scriptIDs{};

	auto &&scriptLoader = GetScriptLoader();
	auto &&scriptProvider = GetScriptProvider();

	for (auto &&[scriptName, entry] : *_hotReloadScriptsPending)
	{
		auto &&[scriptCode, scriptNamespace] = entry;

		auto scriptID = scriptProvider->GetScriptIDByName(scriptName);
		if (scriptID)
		{
			auto &&pendingScripts = GetScriptLoader()->Unload(scriptID);
			scriptIDs.insert(scriptIDs.end(), pendingScripts.begin(), pendingScripts.end());

			if (!scriptProvider->RemoveScript(scriptID)) [[unlikely]]
			{
				_log->Warn("Attempt to remove non-exist script id", scriptID);
			}
		}

		scriptID = scriptProvider->AddScript(scriptName, scriptCode, scriptNamespace);
		scriptIDs.emplace_back(scriptID);
	}

	std::sort(scriptIDs.begin(), scriptIDs.end());
	scriptIDs.erase(std::unique(scriptIDs.begin(), scriptIDs.end()), scriptIDs.end());

	scriptLoader->HotReload(scriptIDs);
	_hotReloadScriptsPending = nullptr;
}

void ModManager::InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept
{
	// eventManager.InstallToScriptEngine(scriptEngine);

	// auto &&table = scriptEngine.CreateTable();

	// //

	// scriptEngine.SetReadonlyGlobal("Events", table);
}
