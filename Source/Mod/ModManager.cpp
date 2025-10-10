/**
 * @file mod/ModManager.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "mod/ModManager.hpp"

#include "Debug/DebugManager.hpp"
#include "data/Constants.hpp"
#include "debug/DebugConsole.hpp"
#include "debug/DebugUtils.hpp"
#include "mod/ModRequirement.hpp"
#include "mod/ScriptEngine.hpp"
#include "mod/ScriptLoader.hpp"
#include "mod/ScriptProvider.hpp"
#include "mod/UnpackagedMod.hpp"
#include "program/Context.hpp"
#include "program/Engine.hpp"
#include "system/LogMicros.hpp"
#include "util/Definitions.hpp"
#include "util/EnumFlag.hpp"
#include "util/Version.hpp"

#include <memory>
#include <optional>
#include <tuple>
#include <vector>

using namespace tudov;

ModManager::ModManager(Context &context) noexcept
    : _log(Log::Get("ModManager")),
      _context(context),
      _loadState(ELoadState::None),
      _updateScriptsPending(nullptr)
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

Log &ModManager::GetLog() noexcept
{
	return *_log;
}

void ModManager::Initialize() noexcept
{
	_modDirectories = {
	    {EGlobalStorageLocation::Application, "Mods"},
	    {EGlobalStorageLocation::User, "Mods"},
	};

	_requiredMods = {
	    ModRequirement(Constants::AppCoreMod, Version(1, 0, 0), 0),
	};

	_updateScriptsPending = nullptr;
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
	std::string_view uid = mod.IMod::GetConfig().uid;

	for (const ModRequirement &modEntry : _requiredMods)
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

	TE_DEBUG("{}", "Loading all required mods ...");

	if (!_requiredMods.empty())
	{
		UnloadMods();
	}

	if (IsNoModMatch())
	{
		TE_DEBUG("{}", "Loaded all required mods: no required mod specified");
		return;
	}

	std::error_code errorCode;
	std::filesystem::recursive_directory_iterator it{"Mods", std::filesystem::directory_options::skip_permission_denied, errorCode}, end;

	while (it != end)
	{
		const std::filesystem::directory_entry &entry = *it;

		// HANDLEING ZIP FILES (PACKAGED MODS)
		// if (entry.is_regular_file() && entry.path().extension() == ".zip")
		// {
		// }

		if (entry.is_directory())
		{
			const std::filesystem::path &dir = entry.path();
			if (_virtualUnpackagedMod->IsValidDirectory(dir))
			{
				const auto &mod = std::make_shared<UnpackagedMod>(*this, dir);
				if (IsModMatched(*mod))
				{
					_loadedMods.emplace_back(mod);
				}
				it.disable_recursion_pending();
			}
		}

		++it;
	}

	for (const std::shared_ptr<Mod> &mod : _loadedMods)
	{
		mod->Load();
	}

	GetScriptLoader().LoadAllScripts();

	TE_DEBUG("{}", "Loaded all required mods");

	EnumFlag::Unmask(_loadState, ELoadState::Loading);
}

void ModManager::LoadModsDeferred() noexcept
{
	EnumFlag::Mask(_loadState, ELoadState::LoadPending);
	EnumFlag::Unmask(_loadState, ELoadState::UnloadPending);

	GetEngine().TriggerLoadPending();
}

void ModManager::UnloadMods()
{
	if (EnumFlag::HasAny(_loadState, ELoadState::Unloading))
	{
		return;
	}
	EnumFlag::Mask(_loadState, ELoadState::Unloading);

	TE_DEBUG("{}", "Unloading all loaded mods ...");

	GetScriptLoader().UnloadAllScripts();

	for (auto &&mod : _loadedMods)
	{
		mod->Unload();
	}
	_loadedMods.clear();

	TE_DEBUG("{}", "Unloaded all loaded mods");

	EnumFlag::Unmask(_loadState, ELoadState::Unloading);
}

void ModManager::UnloadModsDeferred() noexcept
{
	EnumFlag::Mask(_loadState, ELoadState::UnloadPending);
	EnumFlag::Unmask(_loadState, ELoadState::LoadPending);

	GetEngine().TriggerLoadPending();
}

bool ModManager::IsModAvailable(std::string_view modUID) const noexcept
{
	return IsModAvailableImpl(modUID, nullptr);
}

bool ModManager::IsModAvailable(std::string_view modUID, const Version &version) const noexcept
{
	return IsModAvailableImpl(modUID, &version);
}

bool ModManager::IsModAvailableImpl(std::string_view modUID, const Version *version) const noexcept
{
	return true; // TODO not implemented yet
}

std::vector<ModListedEntry> ModManager::ListAvailableMods() const noexcept
{
	std::vector<ModListedEntry> entries;
	// TODO not implemented yet
	return entries;
}

std::shared_ptr<Mod> ModManager::FindLoadedMod(std::string_view modUID) noexcept
{
	for (auto &&mod : _loadedMods)
	{
		if (mod->GetConfig().uid == modUID)
		{
			return mod;
		}
	}
	return nullptr;
}

std::vector<ModRequirement> &ModManager::GetRequiredMods() noexcept
{
	return _requiredMods;
}

const std::vector<ModRequirement> &ModManager::GetRequiredMods() const noexcept
{
	return _requiredMods;
}

bool ModManager::HasUpdateScriptPending() noexcept
{
	return _updateScriptsPending != nullptr;
}

void ModManager::UpdateScriptPending(std::string_view scriptName, TextID scriptTextID, std::string_view scriptModUID)
{
	if (!_updateScriptsPending)
	{
		_updateScriptsPending = std::make_unique<HotReloadScriptsMap>();
	}

	(*_updateScriptsPending)[std::string(scriptName)] = std::make_tuple(scriptTextID, scriptModUID);

	GetEngine().TriggerLoadPending();
}

void ModManager::Update()
{
	for (const std::shared_ptr<Mod> &mod : _loadedMods)
	{
		mod->Update();
	}

	if (EnumFlag::HasAll(_loadState, EnumFlag::BitOr(ELoadState::LoadPending, ELoadState::UnloadPending))) [[unlikely]]
	{
		EnumFlag::Unmask(_loadState, EnumFlag::BitOr(ELoadState::LoadPending, ELoadState::UnloadPending));
	}
	else [[likely]]
	{
		if (EnumFlag::HasAny(_loadState, ELoadState::LoadPending)) [[unlikely]]
		{
			LoadMods();

			EnumFlag::Unmask(_loadState, ELoadState::LoadPending);
		}
		else if (EnumFlag::HasAny(_loadState, ELoadState::UnloadPending)) [[unlikely]]
		{
			UnloadMods();

			EnumFlag::Unmask(_loadState, ELoadState::UnloadPending);
		}
	}

	if (_updateScriptsPending) [[unlikely]]
	{
		UpdateScripts();

		_updateScriptsPending = nullptr;
	}
}

void ModManager::UpdateScripts() noexcept
{
	std::vector<ScriptID> scriptIDs{};

	auto &&scriptLoader = GetScriptLoader();
	auto &&scriptProvider = GetScriptProvider();

	for (auto &&[scriptName, entry] : *_updateScriptsPending)
	{
		auto &&[scriptTextID, scriptModUID] = entry;

		ScriptID scriptID = scriptProvider.GetScriptIDByName(scriptName);
		if (!scriptID) // add new script
		{
			scriptID = scriptProvider.AddScript(scriptName, scriptTextID, scriptModUID);
			scriptIDs.emplace_back(scriptID);

			continue;
		}

		if (scriptTextID != 0) // update script
		{
			auto &&pendingScripts = GetScriptLoader().UnloadScript(scriptID);
			scriptIDs.insert(scriptIDs.end(), pendingScripts.begin(), pendingScripts.end());

			if (!scriptProvider.RemoveScript(scriptID)) [[unlikely]]
			{
				TE_WARN("Attempt to remove non-exist script id", scriptID);
			}

			scriptID = scriptProvider.AddScript(scriptName, scriptTextID, scriptModUID);
			scriptIDs.emplace_back(scriptID);
		}
		else // remove script
		{
			scriptLoader.UnloadScript(scriptID);

			if (!scriptProvider.RemoveScript(scriptID)) [[unlikely]]
			{
				TE_WARN("Attempt to remove non-exist script id", scriptID);
			}
		}
	}

	// When reloading updated scripts, we should also collect all scripts with errors, they might be fixed after this time.
	for (ScriptID scriptID : GetScriptLoader().CollectErrorScripts())
	{
		scriptIDs.emplace_back(scriptID);

		for (ScriptID dependencyScriptID : scriptLoader.GetScriptDependencies(scriptID))
		{
			scriptIDs.emplace_back(dependencyScriptID);
		};
	}

	scriptIDs.erase(std::unique(scriptIDs.begin(), scriptIDs.end()), scriptIDs.end());
	if (!scriptIDs.empty())
	{
		std::sort(scriptIDs.begin(), scriptIDs.end());

		scriptLoader.HotReloadScripts(scriptIDs);
	}

	GetScriptEngine().CollectGarbage();
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
