#pragma once

#include "ScriptEngine.h"
#include "event/DelegateEvent.hpp"
#include "util/Log.h"

#include <sol/table.hpp>

#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace tudov
{
	class ModManager;

	class ScriptLoader
	{
	  private:
		struct Module
		{
		  private:
			bool _isLoaded;
			bool _isLoading;
			sol::table _table;
			sol::protected_function _func;

		  public:
			Module();
			explicit Module(const sol::protected_function &func);

			bool IsLoaded() const;
			bool IsLoading() const;

			const sol::table &LazyLoad(ScriptLoader &scriptLoader);
			const sol::table &ImmediateLoad(ScriptLoader &scriptLoader);
		};

		Log _log;

		std::string _loadingScript;
		std::vector<std::string> _loadingScripts;
		std::unordered_map<std::string_view, Module> _loadedScripts;
		std::unordered_map<std::string_view, std::string> _scriptErrors;
		std::unordered_map<std::string_view, std::string> _scriptErrorsCascaded;

		std::optional<std::reference_wrapper<Module>> LoadImpl(const std::string &scriptName, const std::string_view &code, bool immediate);

	  public:
		ModManager &modManager;

		DelegateEvent onPreLoadAllScripts;
		DelegateEvent onPostLoadAllScripts;

		ScriptLoader(ModManager &modManager);

		const std::string &GetLoadingScript() const;

		void LoadAll();
		void UnloadAll();
		/*
		 * @brief Lazy load a script.
		 * @return Script module.
		 */
		std::optional<std::reference_wrapper<ScriptLoader::Module>> Load(const std::string &scriptName);
		void Unload(const std::string &scriptName);
	};
} // namespace tudov
