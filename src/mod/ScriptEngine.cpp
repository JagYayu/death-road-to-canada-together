#include "ScriptEngine.h"

#include "ModManager.h"
#include "util/StringUtils.hpp"

#include <sol/environment.hpp>
#include <sol/forward.hpp>
#include <sol/load_result.hpp>
#include <sol/sol.hpp>
#include <sol/string_view.hpp>
#include <sol/types.hpp>

#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

using namespace tudov;

int ExceptionHandler(lua_State *L, sol::optional<const std::exception &> exception, sol::string_view desc)
{
	// L is the lua state, which you can wrap in a state_view if necessary
	// maybe_exception will contain exception, if it exists
	// description will either be the what() of the exception or a description saying that we hit the general-case catch(...)
	std::cout << "An exception occurred in a function, here's what it says ";
	if (exception)
	{
		std::cout << "(straight from the exception): ";
		const std::exception &ex = *exception;
		std::cout << ex.what() << std::endl;
	}
	else
	{
		std::cout << "(from the description parameter): ";
		std::cout.write(desc.data(), static_cast<std::streamsize>(desc.size()));
		std::cout << std::endl;
	}

	// you must push 1 element onto the stack to be
	// transported through as the error object in Lua
	// note that Lua -- and 99.5% of all Lua users and libraries -- expects a string
	// so we push a single string (in our case, the description of the error)
	return sol::stack::push(L, desc);
}

struct LuaTableHash
{
	size_t operator()(const sol::table &t) const noexcept
	{
		return std::hash<const void *>{}(t.pointer());
	}
};

struct LuaTableEqual
{
	bool operator()(const sol::table &lhs, const sol::table &rhs) const noexcept
	{
		return lhs.pointer() == rhs.pointer();
	}
};

sol::object MakeReadonlyGlobal(sol::state &lua, const sol::object &obj, std::unordered_map<sol::table, sol::table, LuaTableHash, LuaTableEqual> &visited)
{
	if (obj.get_type() != sol::type::table)
	{
		return obj;
	}

	auto table = obj.as<sol::table>();

	auto it = visited.find(table);
	if (it != visited.end())
	{
		return it->second;
	}

	if (table[sol::metatable_key].valid())
	{
		return obj;
	}

	sol::table proxy = lua.create_table();
	sol::table metatable = lua.create_table(0, 2);

	proxy[sol::metatable_key] = metatable;
	visited[table] = proxy;

	for (auto &&[key, value] : table)
	{
		table[key] = MakeReadonlyGlobal(lua, value, visited);
	}

	metatable["__index"] = table;

	static std::optional<sol::function> throwError = std::nullopt;
	if (!throwError.has_value())
	{
		auto &&result = lua.load(R"(
			error("Attempt to modify read-only global", 2)
		)");
		throwError = result.get<sol::function>();
	}
	metatable["__newindex"] = throwError.value();

	return proxy;
}

ScriptEngine::ScriptEngine(ModManager &modManager)
    : modManager(modManager),
      _lua()
{
	_lua.open_libraries(sol::lib::base);
	_lua.open_libraries(sol::lib::bit32);
	_lua.open_libraries(sol::lib::coroutine);
	_lua.open_libraries(sol::lib::debug);
	_lua.open_libraries(sol::lib::ffi);
	_lua.open_libraries(sol::lib::io);
	_lua.open_libraries(sol::lib::jit);
	_lua.open_libraries(sol::lib::math);
	_lua.open_libraries(sol::lib::string);
	_lua.open_libraries(sol::lib::table);
	_lua.open_libraries(sol::lib::utf8);

	_lua.set_exception_handler(&ExceptionHandler);

	_error = _lua["error"];
	_traceback = _lua["debug"]["traceback"];

	std::unordered_map<sol::table, sol::table, LuaTableHash, LuaTableEqual> visited;
	MakeReadonlyGlobal(_lua, _lua.globals(), visited);
}

sol::state &ScriptEngine::GetState()
{
	return _lua;
}

sol::table ScriptEngine::CreateTable(uint32_t arr, uint32_t hash)
{
	return _lua.create_table(arr, hash);
}
sol::load_result ScriptEngine::LoadFunction(const std::string &name, const std::string_view &code)
{
	return _lua.load(code, name, sol::load_mode::any);
}

void ScriptEngine::ThrowError(const std::string_view &message, uint32_t level)
{
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

void ScriptEngine::Require(const std::string_view &source, const std::string_view &target)
{
	throw std::runtime_error("Permission denied");
}

void ScriptEngine::InitScriptFunc(const std::string &scriptName, sol::protected_function &func)
{
	auto &&namespace_ = GetLuaNamespace(scriptName);

	sol::environment env{_lua, sol::create, _lua.globals()};

	env["print"] = [&](sol::variadic_args args)
	{
		for (auto &&arg : args)
		{
			if (arg.is<std::string>())
			{
				std::cout << arg.as<std::string>() << " ";
			}
			else if (arg.is<int>())
			{
				std::cout << arg.as<int>() << " ";
			}
			else
			{
				std::cout << "[unknown type] ";
			}
		}
		std::cout << std::endl;
	};

	env["require"] = [&](const std::string &scriptName) -> sol::object
	{
		auto &&scriptModule = modManager.scriptLoader.Load(scriptName);
		if (!scriptModule.has_value())
		{
			luaL_error(_lua, "Script module '%s' not found", scriptName.c_str());
			return sol::nil;
		}

		return scriptModule.value().get().LazyLoad(modManager.scriptLoader);
	};

	sol::set_environment(env, func);
}

sol::object ScriptEngine::GetPersistVariable(const std::string_view &key)
{
	auto &&it = _persistVariables.find(key);
	if (it == _persistVariables.end())
	{
		return sol::nil;
	}
	return it->second;
}

void ScriptEngine::SetPersistVariable(const std::string_view &key, const sol::object &value)
{
	_persistVariables[key] = value;
}
