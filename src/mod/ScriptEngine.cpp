#include "ScriptEngine.h"

#include "LuaAPI.h"
#include "ModManager.h"
#include "ScriptLoader.h"
#include "ScriptProvider.h"
#include "sol/table.hpp"
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

ScriptEngine::ScriptEngine(Context &context) noexcept
    : _context(context),
      _log(Log::Get("ScriptEngine")),
      _lua(),
      _luaInit()
{
}

ScriptEngine::~ScriptEngine() noexcept
{
}

Context&ScriptEngine::GetContext()noexcept 
{
	return _context;
}

void ScriptEngine::Initialize() noexcept
{
	if (_luaInit)
	{
		return;
	}
	_luaInit = true;

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

	auto &&scriptLoader = GetScriptLoader();

	_luaThrowModifyReadonlyGlobalError = _lua.load("error('Attempt to modify read-only global', 2)").get<sol::function>();
	_luaInspect = scriptLoader->Load("#lua.inspect")->GetTable().raw_get<sol::function>("inspect");
	auto &&scriptEngineModule = scriptLoader->Load("#lua.ScriptEngine")->GetTable();
	_luaMarkAsLocked = scriptEngineModule.raw_get<sol::function>("markAsLocked");
	_luaPostProcessSandboxing = scriptEngineModule.raw_get<sol::function>("postProcessSandboxing");

	LuaAPI().Install(_lua, _context);

	MakeReadonlyGlobal(_lua.globals());
}

void ScriptEngine::Deinitialize() noexcept
{
	_persistVariables.clear();
	_lua.collect_garbage();
}

sol::state_view &ScriptEngine::GetState()
{
	return _lua;
}

void ScriptEngine::SetReadonlyGlobal(const sol::string_view &key, sol::object value)
{
	if (value.is<sol::table>())
	{
		_lua[key] = GetScriptEngine()->MakeReadonlyGlobal(value.as<sol::table>());
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

sol::table ScriptEngine::CreateTable(uint32_t arr, uint32_t hash) noexcept
{
	return _lua.create_table(arr, hash);
}

sol::load_result ScriptEngine::LoadFunction(const std::string &name, std::string_view code)
{
	return _lua.load(code, name, sol::load_mode::any);
}

int ScriptEngine::ThrowError(std::string_view message) noexcept
{
	lua_pushlstring(_lua, message.data(), message.size());
	return lua_error(_lua);
}

sol::object MakeReadonlyGlobalImpl(sol::state &lua, const sol::object &obj, std::unordered_map<sol::table, sol::table, LuaTableHash, LuaTableEqual> &visited, const sol::function &newindex, const sol::function mark)
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
		table[key] = MakeReadonlyGlobalImpl(lua, value, visited, newindex, mark);
	}

	metatable["__index"] = table;
	metatable["__newindex"] = newindex;
	mark(metatable);

	return proxy;
}

sol::object ScriptEngine::MakeReadonlyGlobal(const sol::object &obj)
{
	Initialize();
	std::unordered_map<sol::table, sol::table, LuaTableHash, LuaTableEqual> visited{};
	return MakeReadonlyGlobalImpl(_lua, obj, visited, _luaThrowModifyReadonlyGlobalError, _luaMarkAsLocked);
}

sol::table &ScriptEngine::GetSandboxedGlobals(std::string_view sandboxKey) noexcept
{
	{
		auto &&it = _sandboxedGlobals.find(sandboxKey);
		if (it != _sandboxedGlobals.end())
		{
			return it->second;
		}
	}

	auto &&globals = _lua.globals();
	auto &&sandbox = CreateTable();

	static constexpr const char *keys[] = {
	    // lua51
	    "_VERSION",
	    "assert",
	    "collectgarbage",
	    "coroutine",
	    "error",
	    "ipairs",
	    "math",
	    "next",
	    "pairs",
	    "pcall",
	    "print",
	    "select",
	    "setmetatable",
	    "string",
	    "table",
	    "tonumber",
	    "tostring",
	    "type",
	    "unpack",
	    "utf8",
	    "xpcall",
	    // C++
	    "engine",
	    "events",
	};

	for (auto &&key : keys)
	{
		sandbox[key] = globals[key];
	}

	sandbox["_G"] = sandbox;

	_luaPostProcessSandboxing(sandbox, globals);

	return _sandboxedGlobals.try_emplace(sandboxKey, sandbox).first->second;
}

void ScriptEngine::InitializeScriptFunction(ScriptID scriptID, const std::string_view scriptName, sol::protected_function &func, std::string_view sandboxKey) noexcept
{
	auto &&namespace_ = GetLuaNamespace(scriptName);

	sol::environment env{_lua, sol::create, sandboxKey.empty() ? _lua.globals().as<sol::table>() : GetSandboxedGlobals(sandboxKey)};

	auto &&log = Log::Get(std::string(scriptName));

	env.set_function("print", [&, log](const sol::variadic_args &args)
	{
		try
		{
			std::string string;
			for (auto &&arg : args)
			{
				if (!string.empty())
				{
					string.append("\t");
				}
				string.append(_luaInspect(arg));
			}
			log->Debug("{}", string.c_str());
		}
		catch (const std::exception &e)
		{
			UnhandledCppException(_log, e);
			throw;
		}
	});

	env.set_function("require", [&, env, log, scriptID](const sol::string_view &targetScriptName) -> sol::object
	{
		try
		{
			auto &&targetScriptID = GetScriptProvider()->GetScriptIDByName(targetScriptName);
			if (targetScriptID)
			{
				auto &&scriptLoader = GetScriptLoader();
				scriptLoader->AddReverseDependency(scriptID, targetScriptID);
				auto &&targetModule = scriptLoader->Load(targetScriptID);
				if (targetModule)
				{
					return targetModule->LazyLoad(*scriptLoader);
				}
			}

			auto &&virtualModule = env[targetScriptName];
			if (virtualModule.valid())
			{
				return virtualModule;
			}

			luaL_error(_lua, "Script module '%s' not found", targetScriptName.data());
			return sol::nil;
		}
		catch (const std::exception &e)
		{
			UnhandledCppException(_log, e);
			throw;
		}
	});

	env.set_function("N_", [&, scriptID](const sol::string_view &str)
	{
		try
		{
			auto &&name = GetScriptProvider()->GetScriptNameByID(scriptID);
			if (!name.has_value())
			{
				luaL_error(_lua, "Script module <%s> not found", scriptID);
				return std::string("");
			}
			std::string_view src = name.value();
			std::string result;
			result.reserve(src.size() + 1 + str.size());
			result.append(src);
			result.push_back('_');
			result.append(str);
			return result;
		}
		catch (const std::exception &e)
		{
			UnhandledCppException(_log, e);
			throw;
		}
	});

	sol::set_environment(env, func);
}

sol::object ScriptEngine::GetPersistVariable(std::string_view key)
{
	auto &&it = _persistVariables.find(key);
	if (it == _persistVariables.end())
	{
		return sol::nil;
	}
	return it->second;
}

void ScriptEngine::SetPersistVariable(std::string_view key, const sol::object &value)
{
	_persistVariables[key] = value;
}
