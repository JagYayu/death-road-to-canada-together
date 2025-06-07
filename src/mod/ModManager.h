#pragma once

#include "Mod.h"
#include "ModEntry.hpp"

#include <filesystem>
#include <vector>

namespace tudov
{
	class ModManager
	{
	private:
		enum class ELoadState
		{
			None = 0,
			AllPending = 1 << 0,
			AllLoading = 1 << 1,
			HotReloading = 1 << 2,
		};

		ELoadState _loadState = ELoadState::None;

		std::vector<std::filesystem::path> _directories = {"mods", "downloadedMods"};
		std::vector<std::shared_ptr<Mod>> _loadedMods;
		std::vector<ModEntry> _modEntries;

		void AddMod(const std::filesystem::path &modRoot);

	public:
		ModManager();
		~ModManager();

		void LoadMods();

		void SetModList(std::vector<ModEntry> modEntries);

		void Update();
	};
}
