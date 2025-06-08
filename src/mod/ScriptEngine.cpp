#include "ScriptEngine.h"
#include "sol/environment.hpp"
#include "sol/forward.hpp"
#include "sol/load_result.hpp"
#include "sol/types.hpp"

#include <exception>
#include <sol/sol.hpp>
#include <stdexcept>

using namespace tudov;

ScriptEngine::ScriptEngine(ModManager &modManager)
    : _modManager(modManager),
      _lua()
{
	_lua.open_libraries();

	_traceback = _lua["debug"]["traceback"];
}

sol::load_result ScriptEngine::LoadFunction(const std::string &name, const std::string &code)
{
	return _lua.load(code, std::string(name), sol::load_mode::any);
}

sol::table ScriptEngine::CreateTable(uint32_t arr, uint32_t hash)
{
	return _lua.create_table(arr, hash);
}

std::string ScriptEngine::Traceback(const std::string &what)
{
	// auto &&a = _traceback(what);
	// return a.get<std::string_view>(0);
	return {};
}

void ScriptEngine::ReleaseModule(sol::table &module)
{
	// auto &&metatable = module[sol::metatable_key];
	// metatable["_released"] = true;
	// metatable["__index"] = [](sol::this_state ls, sol::object, sol::object key)
	// {
	// 	sol::state_view lua{ls};
	// 	std::cout << "Accessed key: ";
	// 	if (key.is<std::string>())
	// 	{
	// 		std::cout << key.as<std::string>();
	// 	}
	// 	else
	// 	{
	// 		std::cout << "<non-string>";
	// 	}
	// 	std::cout << std::endl;

	// 	return sol::lua_nil;
	// };
}

void ScriptEngine::SetScriptEnv(const std::string &scriptName, const sol::function &func)
{
	sol::environment env{_lua, sol::create};

	env["require1"] = []()
	{
		throw std::runtime_error("Permission denied");
	};

	sol::set_environment(env, func);
}
