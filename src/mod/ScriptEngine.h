#pragma once

#include "sol/forward.hpp"
#include "sol/load_result.hpp"
#include "sol/table.hpp"

#include <sol/sol.hpp>

#include <memory>
#include <string_view>
#include <unordered_map>

namespace sol
{
	class state;
}

namespace tudov
{
	class ModManager;

	class ScriptEngine
	{
	  private:
		sol::state _lua;
		sol::function _error;
		sol::table _virtualGlobal;
		sol::function _traceback;
		std::unordered_map<std::string_view, sol::object> _persistVariables;

	  public:
		ModManager &modManager;

		ScriptEngine(ModManager &modManager);

		sol::state& GetState();

		sol::table CreateTable(uint32_t arr = 0, uint32_t hash = 0);
		void ThrowError(const std::string_view &message, uint32_t level);
		sol::load_result LoadFunction(const std::string &name, const std::string_view &code);
		std::string Traceback(const std::string &what);
		void ReleaseModule(sol::table &module);

		void Require(const std::string_view &source, const std::string_view &target);
		void InitScriptFunc(const std::string &scriptName, sol::protected_function &func);

		sol::object GetPersistVariable(const std::string_view &key);
		void SetPersistVariable(const std::string_view &key, const sol::object &value);
	};
} // namespace tudov
