#pragma once

#include "ScriptLoader.h"
#include "ScriptProvider.h"
#include "sol/forward.hpp"
#include "sol/load_result.hpp"
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
		sol::function _error;
		sol::table _virtualGlobal;
		sol::function _traceback;
		sol::function _throwModifyReadonlyGlobalError;
		std::unordered_map<StringView, sol::object> _persistVariables;

	  public:
		ModManager &modManager;
		ScriptLoader scriptLoader;

	  public:
		ScriptEngine(ModManager &modManager);
		~ScriptEngine();

	  private:
		sol::object MakeReadonlyGlobal(const sol::object &obj, std::unordered_map<sol::table, sol::table, LuaTableHash, LuaTableEqual> &visited);
		void OnFatalException(const std::exception&e);

	  public:
		sol::state_view &GetState();

		sol::table CreateTable(uint32_t arr = 0, uint32_t hash = 0);
		void ThrowError(const StringView &message, uint32_t level);
		sol::load_result LoadFunction(const String &name, const StringView &code);

		// void Require(const StringView &source, const StringView &target);
		void InitScriptFunc(const String &scriptName, sol::protected_function &func);

		sol::object GetPersistVariable(const StringView &key);
		void SetPersistVariable(const StringView &key, const sol::object &value);
	};
} // namespace tudov
