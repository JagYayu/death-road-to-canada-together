#include "ModManager.h"

#include "ScriptEngine.h"
#include "ScriptProvider.h"
#include "UnpackagedMod.h"
#include "mod/ModEntry.hpp"
#include "util/Defs.h"

#include <memory>
#include <vector>

using namespace tudov;

ModManager::ModManager(Engine &engine)
    : engine(engine),
      scriptEngine(*this),
      scriptProvider(*this),
      eventManager(*this),
      _log(Log::Get("ModManager")),
      _loadState(ELoadState::None)
{
}

ModManager::~ModManager()
{
	UnloadMods();
}

void ModManager::Initialize()
{
	_directories = {
	    "mods",
	    "downloadedMods",
	};
	_requiredMods = {
	    ModEntry("dr2c", Version(1, 0, 0), 0),
	};

	scriptEngine.Initialize();
	eventManager.InstallToScriptEngine(scriptEngine);
	eventManager.AttachToScriptLoader(scriptEngine.scriptLoader);
}

void ModManager::Deinitialize()
{
	eventManager.DetachFromScriptLoader(scriptEngine.scriptLoader);
	eventManager.UninstallFromScriptEngine(scriptEngine);
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
			if (UnpackagedMod::IsValidDirectory(dir))
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

	scriptEngine.scriptLoader.LoadAll();
	scriptEngine.CollectGarbage();

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

	scriptEngine.scriptLoader.UnloadAll();

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

void ModManager::HotReloadScriptPending(std::string scriptName, std::string scriptCode)
{
	if (!_hotReloadScriptsPending)
	{
		_hotReloadScriptsPending = std::make_unique<std::unordered_map<std::string, std::string>>();
	}
	(*_hotReloadScriptsPending)[scriptName] = scriptCode;
}

void ModManager::Update()
{
	eventManager.update->Invoke();

	if (_hotReloadScriptsPending)
	{
		std::vector<ScriptID> scriptIDs{};

		for (auto &&[scriptName, scriptCode] : *_hotReloadScriptsPending)
		{
			auto scriptID = scriptProvider.GetScriptIDByName(scriptName);

			auto &&pendingScripts = scriptEngine.scriptLoader.Unload(scriptID);
			scriptIDs.insert(scriptIDs.end(), pendingScripts.begin(), pendingScripts.end());

			auto &&namespace_ = scriptProvider.GetScriptNamespace(scriptID);
			scriptProvider.RemoveScript(scriptID);
			scriptID = scriptProvider.AddScript(scriptName, scriptCode, namespace_);

			scriptIDs.emplace_back(scriptID);
		}

		std::sort(scriptIDs.begin(), scriptIDs.end());
		scriptIDs.erase(std::unique(scriptIDs.begin(), scriptIDs.end()), scriptIDs.end());

		scriptEngine.scriptLoader.HotReload(scriptIDs);
		_hotReloadScriptsPending = nullptr;
	}
}
