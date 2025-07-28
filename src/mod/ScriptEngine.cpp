#include "mod/ScriptEngine.hpp"

#include "mod/LuaAPI.hpp"
#include "mod/ModManager.hpp"
#include "mod/ScriptLoader.hpp"
#include "mod/ScriptProvider.hpp"
#include "util/Definitions.hpp"
#include "util/Utils.hpp"

#include "sol/environment.hpp"
#include "sol/forward.hpp"
#include "sol/load_result.hpp"
#include "sol/string_view.hpp"
#include "sol/table.hpp"
#include "sol/types.hpp"
#include "sol/variadic_args.hpp"

#include <cassert>
#include <corecrt_terminate.h>
#include <unordered_map>

using namespace tudov;

ScriptEngine::ScriptEngine(Context &context) noexcept
    : _context(context),
      _log(Log::Get("ScriptEngine")),
      _lua(),
      _luaInit()
{
}

Log &ScriptEngine::GetLog() noexcept
{
	return *_log;
}

Context &ScriptEngine::GetContext() noexcept
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
	_lua.open_libraries(sol::lib::package);
	_lua.open_libraries(sol::lib::string);
	_lua.open_libraries(sol::lib::table);
	_lua.open_libraries(sol::lib::utf8);

	auto &&scriptLoader = GetScriptLoader();

	_luaThrowModifyReadonlyGlobalError = _lua.load("error('Attempt to modify read-only global', 2)").get<sol::function>();

	{
		auto &&inspectModule = scriptLoader.Load("#lua.inspect");
		_luaInspect = inspectModule->GetTable().raw_get<sol::function>("inspect");
	}

	{
		auto &&engineModule = scriptLoader.Load("#lua.ScriptEngine");
		auto &&scriptEngineModule = engineModule->GetTable();

		scriptEngineModule.raw_get<sol::function>("initialize")();
		_luaMarkAsLocked = scriptEngineModule.raw_get<sol::function>("markAsLocked");
		_luaPostProcessModGlobals = scriptEngineModule.raw_get<sol::function>("postProcessModGlobals");
		_luaPostProcessScriptGlobals = scriptEngineModule.raw_get<sol::function>("postProcessScriptGlobals");
	}

	GetLuaAPI().Install(_lua, _context);

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
		_lua[key] = GetScriptEngine().MakeReadonlyGlobal(value.as<sol::table>());
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

sol::object MakeReadonlyGlobalImpl(sol::state &lua, sol::object obj, std::unordered_map<sol::table, sol::table, LuaTableHash, LuaTableEqual> &visited, const sol::function &newindex, const sol::function mark)
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

sol::object ScriptEngine::MakeReadonlyGlobal(sol::object obj)
{
	Initialize();
	std::unordered_map<sol::table, sol::table, LuaTableHash, LuaTableEqual> visited{};
	return MakeReadonlyGlobalImpl(_lua, obj, visited, _luaThrowModifyReadonlyGlobalError, _luaMarkAsLocked);
}

sol::table &ScriptEngine::GetModGlobals(std::string_view modUID, bool sandboxed) noexcept
{
	if (auto &&it = _modGlobals.find(modUID); it != _modGlobals.end())
	{
		return it->second;
	}

	auto &&luaGlobals = _lua.globals();
	auto &&modGlobals = CreateTable();

	static constexpr const char *keys[] = {
	    // lua51
	    "_VERSION",
	    "assert",
	    "collectgarbage",
	    "coroutine",
	    "error",
	    "ipairs",
	    "load",
	    "loadstring",
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
	    "images",
	};

	for (auto &&key : keys)
	{
		modGlobals[key] = luaGlobals[key];
	}

	modGlobals["_G"] = modGlobals;

	_luaPostProcessModGlobals(modUID, sandboxed, modGlobals, luaGlobals);

	return _modGlobals.try_emplace(modUID, modGlobals).first->second;
}

void ScriptEngine::InitializeScript(ScriptID scriptID, std::string_view scriptName, std::string_view modUID, bool sandboxed, sol::protected_function &func) noexcept
{
	sol::environment scriptGlobals{_lua, sol::create, GetModGlobals(modUID, sandboxed)};

	if (!scriptGlobals.valid()) [[unlikely]]
	{
		_log->Error("Failed to create Lua environment!");
		return;
	}

	scriptGlobals["log"] = Log::Get(scriptName);

	scriptGlobals.set_function("persist", [this, scriptName](sol::string_view key, sol::object defaultValue, const sol::function &getter)
	{
		if (defaultValue == sol::nil) [[unlikely]]
		{
			ThrowError("Default value could not be nil");
			return sol::object(sol::nil);
		}

		return RegisterPersistVariable(scriptName, key, defaultValue, getter);
	});

	scriptGlobals.set_function("print", [this, scriptName](const sol::variadic_args &args)
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
			Log::Get(scriptName)->Debug("{}", string.c_str());
		}
		catch (const std::exception &e)
		{
			UnhandledCppException(_log, e);
		}
	});

	scriptGlobals.set_function("require", [this, scriptGlobals, scriptID](const sol::string_view &targetScriptName) -> sol::object
	{
		try
		{
			auto &&scriptProvider = GetScriptProvider();
			auto targetScriptID = scriptProvider.GetScriptIDByName(targetScriptName);
			if (targetScriptID)
			{
				auto &&scriptLoader = GetScriptLoader();
				if (!scriptProvider.IsStaticScript(targetScriptID))
				{
					scriptLoader.AddReverseDependency(scriptID, targetScriptID);
				}

				auto &&targetModule = scriptLoader.Load(targetScriptID);
				if (targetModule)
				{
					return targetModule->LazyLoad(scriptLoader);
				}
			}

			auto &&virtualModule = scriptGlobals[targetScriptName];
			if (virtualModule.valid())
			{
				return virtualModule;
			}

			IScriptEngine::ThrowError("Script module '{}' not found", targetScriptName.data());
		}
		catch (const std::exception &e)
		{
			UnhandledCppException(_log, e);
		}

		return sol::nil;
	});

	_luaPostProcessScriptGlobals(scriptID, scriptName, modUID, sandboxed, func, scriptGlobals, _lua.globals());

	sol::set_environment(scriptGlobals, func);
}

void ScriptEngine::DeinitializeScript(ScriptID scriptID, std::string_view scriptName)
{
	auto &&it = _persistVariables.find(scriptName.data());
	if (it != _persistVariables.end())
	{
		for (auto &&[_, variable] : it->second)
		{
			auto result = variable.getter();
			if (result.valid())
			{
				auto &&value = result.get<sol::object>();
				if (value != sol::nil)
				{
					variable.value = value;
					variable.getter = sol::nil;
				}
			}
		}
	}
}

sol::object ScriptEngine::RegisterPersistVariable(std::string_view scriptName, std::string_view key, sol::object defaultValue, const sol::function &getter)
{
	if (defaultValue == sol::nil) [[unlikely]]
	{
		Warn("\"{}\" attempt to provide nil value to default persist variable '{}'", scriptName, key);
	}

	if (!_persistVariables.contains(scriptName.data()))
	{
		_persistVariables[scriptName.data()] = {};
	}

	auto &&variable = _persistVariables[scriptName.data()][key.data()];
	if (!variable.value.valid())
	{
		variable.value = defaultValue;
	}
	variable.getter = getter;

	return variable.value;
}

void ScriptEngine::ClearPersistVariables()
{
	_persistVariables.clear();
}
