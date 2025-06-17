#pragma once

#include "ScriptProvider.h"
#include "event/DelegateEvent.hpp"
#include "util/Defs.h"
#include "util/Log.h"
#include "util/StringUtils.hpp"

#include <sol/table.hpp>

#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace tudov
{
	class ScriptEngine;

	class ScriptLoader
	{
	  private:
		struct Module
		{
		  private:
			ScriptID _scriptID;
			sol::protected_function _func;
			bool _fullyLoaded;
			sol::table _table;

		  public:
			Module();
			explicit Module(ScriptID scriptID, const sol::protected_function &func);

			bool IsLazyLoaded() const;
			bool IsFullyLoaded() const;

			const sol::table &GetTable();
			const sol::table &RawLoad(ScriptLoader &scriptLoader);
			const sol::table &LazyLoad(ScriptLoader &scriptLoader);
			const sol::table &FullLoad(ScriptLoader &scriptLoader);
		};

	  public:
		using ScriptID = ScriptID;

	  private:
		std::shared_ptr<Log> _log;
		ScriptID _loadingScript;
		std::vector<std::string> _loadingScripts;
		std::unordered_map<ScriptID, std::shared_ptr<Module>> _scriptModules;
		std::unordered_map<ScriptID, std::unordered_set<ScriptID>> _scriptReverseDependencies;
		std::unordered_map<ScriptID, std::string> _scriptErrors;
		std::unordered_map<ScriptID, std::string> _scriptErrorsCascaded;

		std::shared_ptr<Module> LoadImpl(ScriptID scriptID, std::string_view scriptName, std::string_view code, std::string_view mod);
		void UnloadImpl(ScriptID scriptID, std::vector<ScriptID> &unloadedScripts);

	  public:
		ScriptEngine &scriptEngine;

		DelegateEvent<> onPreLoadAllScripts;
		DelegateEvent<> onPostLoadAllScripts;
		DelegateEvent<const std::vector<ScriptID> &> onPreHotReloadScripts;
		DelegateEvent<const std::vector<ScriptID> &> onPostHotReloadScripts;
		/*
		 * Called right after a script was `RawLoaded` or `LazyLoaded`.
		 */
		DelegateEvent<ScriptID> onLoadedScript;
		/*
		 * Called before a script was unloaded.
		 */
		DelegateEvent<ScriptID> onUnloadScript;

	  public:
		ScriptLoader(ScriptEngine &scriptEngine) noexcept;
		~ScriptLoader() noexcept;

		ScriptID GetLoadingScriptID() const noexcept;
		std::optional<std::string_view> GetLoadingScriptName() const noexcept;

		std::vector<ScriptID> GetDependencies(ScriptID scriptID) const;
		void AddReverseDependency(ScriptID source, ScriptID target);

		void LoadAll();
		void UnloadAll();
		/*
		 * Try load script's module, do nothing if already loaded.
		 */
		std::shared_ptr<ScriptLoader::Module> Load(ScriptID scriptID);
		std::shared_ptr<ScriptLoader::Module> Load(std::string_view scriptName);
		std::vector<ScriptID> Unload(ScriptID scriptID);
		void HotReload(const std::vector<ScriptID> &scriptIDs);
		void ProcessFullLoads();
	};
} // namespace tudov
