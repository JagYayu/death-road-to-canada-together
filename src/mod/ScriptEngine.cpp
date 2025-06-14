#include "ScriptEngine.h"

#include "ModManager.h"
#include "ScriptLoader.h"
#include "ScriptProvider.h"
#include "program/Engine.h"
#include "util/Defs.h"
#include "util/StringUtils.hpp"
#include "util/Utils.hpp"

#include <cassert>
#include <corecrt_terminate.h>
#include <sol/environment.hpp>
#include <sol/forward.hpp>
#include <sol/load_result.hpp>
#include <sol/sol.hpp>
#include <sol/string_view.hpp>
#include <sol/types.hpp>
#include <sol/variadic_args.hpp>

#include <unordered_map>

using namespace tudov;

ScriptEngine::ScriptEngine(ModManager &modManager)
    : modManager(modManager),
      scriptLoader(*this),
      _log(Log::Get("ScriptEngine")),
      _lua()
{
}

ScriptEngine::~ScriptEngine()
{
	_persistVariables.clear();
	_virtualGlobal = sol::nil;
	_lua.collect_garbage();
}

void ScriptEngine::Initialize()
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

	_throwModifyReadonlyGlobalError = _lua.load("error('Attempt to modify read-only global', 2)").get<sol::function>();

	MakeReadonlyGlobal(_lua.globals());
}

sol::state_view &ScriptEngine::GetState()
{
	return _lua;
}

void ScriptEngine::SetReadonlyGlobal(const sol::string_view &key, const sol::table &value)
{
	if (value.is<sol::table>())
	{
		_lua[key] = modManager.scriptEngine.MakeReadonlyGlobal(value.as<sol::table>());
	}
	else
	{
		_lua[key] = value;
	}
}

void ScriptEngine::CollectGarbage()
{
	_lua.collect_garbage();
}

size_t ScriptEngine::GetMemory() const noexcept
{
	return _lua.memory_used();
}

sol::table ScriptEngine::CreateTable(uint32_t arr, uint32_t hash)
{
	return _lua.create_table(arr, hash);
}

sol::load_result ScriptEngine::LoadFunction(const String &name, StringView code)
{
	return _lua.load(code, name, sol::load_mode::any);
}

int ScriptEngine::ThrowError(StringView message)
{
	String str{message};
	lua_pushlstring(_lua, str.data(), str.size());
	return lua_error(_lua);
}

// void ScriptEngine::Require(StringView source, StringView target)
// {
// 	throw std::runtime_error("Permission denied");
// }

sol::object MakeReadonlyGlobalImpl(sol::state &lua, const sol::object &obj, const sol::function &newindex, std::unordered_map<sol::table, sol::table, LuaTableHash, LuaTableEqual> &visited)
{
	if (obj.get_type() != sol::type::table)
	{
		return obj;
	}

	auto &&table = obj.as<sol::table>();

	auto &&it = visited.find(table);
	if (it != visited.end())
	{
		return it->second;
	}

	if (table[sol::metatable_key].valid())
	{
		return obj;
	}

	sol::table &&proxy = lua.create_table();
	sol::table &&metatable = lua.create_table(0, 2);

	proxy[sol::metatable_key] = metatable;
	visited[table] = proxy;

	for (auto &&[key, value] : table)
	{
		table[key] = MakeReadonlyGlobalImpl(lua, value, newindex, visited);
	}

	metatable["__index"] = table;
	metatable["__newindex"] = newindex;

	return proxy;
}

sol::object ScriptEngine::MakeReadonlyGlobal(const sol::object &obj)
{
	UnorderedMap<sol::table, sol::table, LuaTableHash, LuaTableEqual> visited;
	return MakeReadonlyGlobalImpl(_lua, obj, _throwModifyReadonlyGlobalError, visited);
}

void ScriptEngine::InitScriptFunc(ScriptID scriptID, const StringView scriptName, sol::protected_function &func) noexcept
{
	auto &&namespace_ = GetLuaNamespace(scriptName);

	sol::environment env{_lua, sol::create, _lua.globals()};

	auto &&log = Log::Get(String(scriptName));

	env["print"] = [&, log](sol::variadic_args args)
	{
		try
		{
			auto &&a = modManager.scriptProvider.GetScriptIDByName("#lua.inspect");
			auto &&b = scriptLoader.Load(a);
			auto &&inspect = b->GetTable().raw_get<sol::function>("inspect");
			String string;
			for (auto &&arg : args)
			{
				if (!string.empty())
				{
					string.append("\t");
				}
				string.append(inspect(arg));
			}
			log->Debug("{}", string.c_str());
		}
		catch (const std::exception &e)
		{
			UnhandledCppException(_log, e);
			throw;
		}
	};

	env["require"] = [&, log, scriptID](sol::string_view targetScriptName) -> sol::object
	{
		try
		{
			auto &&targetScriptID = modManager.scriptProvider.GetScriptIDByName(targetScriptName);

			auto &&targetModule = scriptLoader.Load(targetScriptID);
			if (!targetModule)
			{
				luaL_error(_lua, "Script module '%s' not found", targetScriptName);
				return sol::nil;
			}

			scriptLoader.AddReverseDependency(scriptID, targetScriptID);

			return targetModule->LazyLoad(scriptLoader);
		}
		catch (const std::exception &e)
		{
			UnhandledCppException(_log, e);
			throw;
		}
	};

	sol::set_environment(env, func);
}

sol::object ScriptEngine::GetPersistVariable(StringView key)
{
	auto &&it = _persistVariables.find(key);
	if (it == _persistVariables.end())
	{
		return sol::nil;
	}
	return it->second;
}

void ScriptEngine::SetPersistVariable(StringView key, const sol::object &value)
{
	_persistVariables[key] = value;
}
