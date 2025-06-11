#pragma once

#include "ScriptLoader.h"
#include "ScriptProvider.h"
#include "sol/forward.hpp"
#include "sol/load_result.hpp"
#include "sol/string_view.hpp"
#include "sol/table.hpp"
#include "util/Defs.h"
#include "util/Utils.hpp"

#include <sol/sol.hpp>

#include <memory>
#include <string_view>
#include <unordered_map>

namespace tudov
{
	class ModManager;

	class ScriptEngine
	{
	  private:
		SharedPtr<Log> _log;
		sol::state _lua;
		sol::table _virtualGlobal;
		sol::function _throwModifyReadonlyGlobalError;
		std::unordered_map<StringView, sol::object> _persistVariables;

	  public:
		ModManager &modManager;
		ScriptLoader scriptLoader;

	  public:
		ScriptEngine(ModManager &modManager);
		~ScriptEngine();

	  private:
		void OnFatalException(const std::exception &e);

	  public:
		void Initialize();

		sol::state_view &GetState();

		void Set(const sol::string_view &key, const sol::object &value);
		void CollectGarbage();
		sol::table CreateTable(UInt32 arr = 0, UInt32 hash = 0);
		void ThrowError(StringView message);
		sol::load_result LoadFunction(const String &name, StringView code);

		sol::object MakeReadonlyGlobal(const sol::object &obj);

		// void Require(StringView source, StringView target);
		void InitScriptFunc(ScriptProvider::ScriptID scriptID, StringView scriptName, sol::protected_function &func);

		sol::object GetPersistVariable(StringView key);
		void SetPersistVariable(StringView key, const sol::object &value);
	};
} // namespace tudov
