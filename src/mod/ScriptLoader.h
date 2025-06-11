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
		enum class LoadType
		{
			Unsafe,
			Lazy,
			Immediate,
		};

		struct Module
		{
		  private:
			sol::table _table;
			sol::protected_function _func;

		  public:
			Module();
			explicit Module(const sol::protected_function &func);

			bool IsLoaded() const;

			const sol::table &GetTable();
			const sol::table &RawLoad();
			const sol::table &LazyLoad(ScriptLoader &scriptLoader);
			const sol::table &ImmediateLoad(ScriptLoader &scriptLoader);
		};

	  public:
		using ScriptID = ScriptProvider::ScriptID;

	  private:
		SharedPtr<Log> _log;
		ScriptID _loadingScript;
		Vector<String> _loadingScripts;
		UnorderedMap<ScriptID, SharedPtr<Module>> _scriptModules;
		UnorderedMap<ScriptID, UnorderedSet<ScriptID>> _scriptReverseDependencies;
		UnorderedMap<ScriptID, String> _scriptErrors;
		UnorderedMap<ScriptID, String> _scriptErrorsCascaded;

		SharedPtr<Module> LoadImpl(ScriptID scriptID, StringView scriptName, StringView code);
		void UnloadImpl(ScriptID scriptID, Vector<ScriptID> &unloadedScripts);

	  public:
		ScriptEngine &scriptEngine;

		DelegateEvent<> onPreLoadAllScripts;
		DelegateEvent<> onPostLoadAllScripts;

	  public:
		ScriptLoader(ScriptEngine &scriptEngine) noexcept;
		~ScriptLoader() noexcept;

		ScriptID GetLoadingScript() const noexcept;

		Vector<ScriptID> GetDependencies(ScriptID scriptID) const;
		void AddReverseDependency(ScriptID source, ScriptID target);

		void LoadAll();
		void UnloadAll();
		SharedPtr<ScriptLoader::Module> Load(ScriptID scriptID);
		Vector<ScriptID> Unload(ScriptID scriptID);
		void ProcessImmediateLoads();
	};
} // namespace tudov
