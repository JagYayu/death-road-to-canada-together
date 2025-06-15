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

		Vector<std::filesystem::path> _directories;
		Vector<SharedPtr<Mod>> _loadedMods;
		Vector<ModEntry> _requiredMods;

		UniquePtr<UnorderedMap<String, String>> _hotReloadScriptsPending;

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

		WeakPtr<Mod> GetLoadedMod(StringView namespace_) noexcept;

		Vector<ModEntry> &GetRequiredMods() noexcept;
		const Vector<ModEntry> &GetRequiredMods() const noexcept;

		void HotReloadScriptPending(String scriptName, String scriptCode);

		void Update();
	};
} // namespace tudov
