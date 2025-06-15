#pragma once

#include "Mod.h"
#include "ModEntry.hpp"
#include "ScriptEngine.h"
#include "ScriptLoader.h"
#include "ScriptProvider.h"
#include "event/EventManager.h"
#include "util/Defs.h"
#include "util/Log.h"

#include <filesystem>
#include <vector>

namespace tudov
{
	class Engine;

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

		SharedPtr<Log> _log;
		ELoadState _loadState;

		std::vector<std::filesystem::path> _directories;
		std::vector<SharedPtr<Mod>> _loadedMods;
		std::vector<ModEntry> _requiredMods;

		UniquePtr<std::unordered_map<std::string, std::string>> _hotReloadScriptsPending;

	  public:
		Engine &engine;
		ScriptEngine scriptEngine;
		ScriptProvider scriptProvider;
		EventManager eventManager;

		void AddMod(const std::filesystem::path &modRoot);

	  public:
		ModManager(Engine &engine);
		~ModManager();

		void Initialize();
		void Deinitialize();

		bool IsNoModMatch() const;
		bool IsModMatched(const Mod &mod) const;

		void LoadMods();
		void UnloadMods();

		WeakPtr<Mod> GetLoadedMod(std::string_view namespace_) noexcept;

		std::vector<ModEntry> &GetRequiredMods() noexcept;
		const std::vector<ModEntry> &GetRequiredMods() const noexcept;

		void HotReloadScriptPending(std::string scriptName, std::string scriptCode);

		void Update();
	};
} // namespace tudov
