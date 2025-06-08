#pragma once

#include "ScriptEngine.h"
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
			sol::function _func;

		  public:
			Module();
			explicit Module(const sol::function &func);

			bool IsLoaded() const;
			bool IsLoading() const;

			const sol::table &LazyLoad(ScriptLoader &scriptLoader);
			const sol::table &ImmediateLoad(ScriptLoader &scriptLoader);
		};

		Log _log;

		std::string _loadingScript;
		std::vector<std::string> _loadingScripts;
		std::unordered_map<std::string, Module> _loadedScripts;
		std::unordered_map<std::string, std::string> _scriptErrors;
		std::unordered_map<std::string, std::string> _scriptErrorsCascaded;

		bool LoadScript(const std::string &scriptName, const std::string &code);

	  public:
		ModManager &modManager;

		ScriptLoader(ModManager &modManager);

		const std::string &GetLoadingScript() const;

		void LoadAll();
		void UnloadAll();
		bool Load(const std::string &scriptName);
		void Unload(const std::string &scriptName);
	};
} // namespace tudov
