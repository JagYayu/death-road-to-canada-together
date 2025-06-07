#include "ModManager.h"

#include "UnpackagedMod.h"

#include<vector>
#include <string>
#include <iostream>

using namespace tudov;

ModManager::ModManager()
{
}

ModManager::~ModManager()
{
}

void ModManager::AddMod(const std::filesystem::path &modRoot)
{
}

void ModManager::LoadMods()
{
	for (auto &&mod : _loadedMods)
	{
		/* code */
	}

	std::error_code errorCode;
	std::filesystem::recursive_directory_iterator it("mods", std::filesystem::directory_options::skip_permission_denied, errorCode), end;

	while (it != end)
	{
		const std::filesystem::directory_entry &entry = *it;

		// HANDLEING ZIP FILES (PACKAGED MODS)
		// if (entry.is_regular_file() && entry.path().extension() == ".zip")
		// {
		// 	result.push_back(entry.path());
		// 	++it;
		// 	continue;
		// }

		if (entry.is_directory())
		{
			auto &&path = entry.path();
			std::filesystem::path modJsonPath = path / "Mod.json";
			if (std::filesystem::exists(modJsonPath))
			{
				auto &&mod = std::make_shared<UnpackagedMod>(*this, path);
				_loadedMods.emplace_back(mod);
				it.disable_recursion_pending();
			}
		}

		++it;
	}
}

void ModManager::SetModList(std::vector<ModEntry> modEntries)
{
}

void ModManager::Update()
{
}
