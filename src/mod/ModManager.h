#pragma once

#include "Mod.h"
#include "ModEntry.hpp"
#include "ScriptEngine.h"
#include "ScriptLoader.h"
#include "ScriptProvider.h"
#include "util/Log.h"

#include <filesystem>
#include <vector>

namespace tudov
{
	class ScriptLoader;
	class ScriptProvider;

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

		Log _log;
		ELoadState _loadState;

		std::vector<std::filesystem::path> _directories;
		std::vector<std::shared_ptr<Mod>> _mountedMods;
		std::vector<ModEntry> _requiredMods;

	  public:
		ScriptEngine scriptEngine;
		ScriptLoader scriptLoader;
		ScriptProvider scriptProvider;

		void AddMod(const std::filesystem::path &modRoot);

	  public:
		ModManager();
		~ModManager();

		bool IsNoModMatch() const;
		bool IsModMatched(const Mod &mod) const;

		void LoadMods();
		void UnloadMods();

		void SetModList(std::vector<ModEntry> requiredMods);

		void Update();
	};
} // namespace tudov
