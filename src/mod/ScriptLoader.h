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

		SharedPtr<Log> _log;

		Optional<String> _loadingScript;
		Vector<String> _loadingScripts;
		UnorderedMap<String, Module, StringSVHash, StringSVEqual> _loadedScripts;
		UnorderedMap<StringView, String, StringSVHash, StringSVEqual> _scriptErrors;
		UnorderedMap<StringView, String, StringSVHash, StringSVEqual> _scriptErrorsCascaded;

		Optional<Reference<Module>> LoadImpl(const String &scriptName, const StringView &code, bool immediate);

	  public:
		ScriptEngine &scriptEngine;

		DelegateEvent<> onPreLoadAllScripts;
		DelegateEvent<> onPostLoadAllScripts;

	  public:
		ScriptLoader(ScriptEngine &scriptEngine) noexcept;
		~ScriptLoader() noexcept;

		Optional<String> GetLoadingScript() const;

		void LoadAll();
		void UnloadAll();
		/*
		 * @brief Lazy load a script.
		 * @return Script module.
		 */
		Optional<Reference<ScriptLoader::Module>> Load(const String &scriptName);
		void Unload(const String &scriptName);
	};
} // namespace tudov
