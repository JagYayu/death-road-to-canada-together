#include "ModManager.h"

#include "ScriptEngine.h"
#include "UnpackagedMod.h"
#include "mod/ModEntry.hpp"

#include <vector>

using namespace tudov;

ModManager::ModManager()
    : scriptEngine(*this),
      scriptLoader(*this),
      scriptProvider(*this),
      _log("ModManager"),
      _loadState(ELoadState::None)
{
	_directories = {
	    "mods",
	    "downloadedMods",
	};
	_requiredMods = {
	    ModEntry("Core", Version(1, 0, 0), 0),
	};
}

ModManager::~ModManager()
{
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
	_log.Debug("Loading all required mods ...");

	if (!_requiredMods.empty())
	{
		UnloadMods();
	}

	if (IsNoModMatch())
	{
		_log.Debug("Loaded all required mods: no required mod specified");
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
					_mountedMods.emplace_back(mod);
				}
				it.disable_recursion_pending();
			}
		}

		++it;
	}

	for (auto &&mod : _mountedMods)
	{
		mod->Load();
	}

	scriptLoader.LoadAll();

	_log.Debug("Loaded all required mods");
}

void ModManager::UnloadMods()
{
	_log.Debug("Unloading all mounted mods ...");

	scriptLoader.UnloadAll();

	for (auto &&mod : _mountedMods)
	{
		mod->Unload();
	}
	_mountedMods.clear();

	_log.Debug("Unloaded all mounted mods");
}

void ModManager::SetModList(std::vector<ModEntry> modEntries)
{
}

void ModManager::Update()
{
}
