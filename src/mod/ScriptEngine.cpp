/**
 * @file mod/ScriptEngine.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "mod/ScriptEngine.hpp"

#include "event/CoreEvents.hpp"
#include "event/CoreEventsData.hpp"
#include "event/RuntimeEvent.hpp"
#include "mod/LuaBindings.hpp"
#include "mod/ModManager.hpp"
#include "mod/ScriptLoader.hpp"
#include "mod/ScriptModule.hpp"
#include "mod/ScriptProvider.hpp"
#include "system/LogMicros.hpp"
#include "util/Definitions.hpp"
#include "util/LuaUtils.hpp"
#include "util/Utils.hpp"

#include "sol/environment.hpp"
#include "sol/forward.hpp"
#include "sol/load_result.hpp"
#include "sol/protected_function_result.hpp"
#include "sol/string_view.hpp"
#include "sol/table.hpp"
#include "sol/types.hpp"
#include "sol/variadic_args.hpp"

#include <array>
#include <corecrt_terminate.h>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

using namespace tudov;

constexpr std::array<std::string_view, 6> preRequiredStdModules = {
    "ffi",
    "jit.profile",
    "jit.util",
    "string.buffer",
    "table.clear",
    "table.new",
};

std::unordered_set<std::string_view> permissiveModules = {
    "#ScriptEngine",
};

void ScriptEngine::PersistVariable::Save() noexcept
{
	if (getter.valid())
	{
		sol::protected_function_result result = getter();
		if (result.valid())
		{
			auto value = result.get<sol::object>();
			if (value != sol::nil)
			{
				this->value = value;
				getter = sol::nil;
			}
		}
	}
}

ScriptEngine::ScriptEngine(Context &context) noexcept
    : _context(context),
      _log(Log::Get("ScriptEngine")),
      _lua({}),
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

void ScriptEngine::PreInitialize() noexcept
{
	IScriptLoader &scriptLoader = GetScriptLoader();

	scriptLoader.GetOnUnloadScript() += [this](ScriptID scriptID, std::string_view scriptName) -> void
	{
		DeinitializeScript(scriptID, scriptName);
	};
}

void ScriptEngine::PostDeinitialize() noexcept
{
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
	_lua.open_libraries(sol::lib::os);
	_lua.open_libraries(sol::lib::package);
	_lua.open_libraries(sol::lib::string);
	_lua.open_libraries(sol::lib::table);
	_lua.open_libraries(sol::lib::utf8);

	GetLuaBindings().Install(_lua, _context);

	IScriptLoader &scriptLoader = GetScriptLoader();

	_luaThrowModifyReadonlyGlobalError = _lua.load("error('Attempt to modify read-only global', 2)").get<sol::protected_function>();

	_luaSTDRequire = _lua["require"];
	_lua["require"] = [this](sol::string_view targetScriptName) -> sol::object
	{
		return LuaRequire(targetScriptName, nullptr);
	};

	// Load lua StackTracePlus
	if (auto &&stackTracePlusModule = scriptLoader.Load("#StackTracePlus"); stackTracePlusModule != nullptr)
	{
		_luaStackTrackPlus = stackTracePlusModule->GetTable();

		AssertLuaValue(_luaStackTrackPlus["stacktrace"], "#StackTracePlus.stacktrace");

		sol::protected_function::set_default_handler(_luaStackTrackPlus["stacktrace"]);
		_lua["debug"]["trackback"] = _luaStackTrackPlus["stacktrace"];
	}
	else
	{
		Fatal("Could not find core lua module \"#StackTracePlus\"!");
	}

	// Load lua inspect
	if (auto &&inspectModule = scriptLoader.Load("#inspect"); inspectModule != nullptr)
	{
		_luaInspect = inspectModule->GetTable()["inspect"];

		AssertLuaValue(_luaInspect, "#inspect.inspect");
	}
	else
	{
		Fatal("Could not find core lua module \"#inspect\"!");
	}

	// Load lua ScriptEngine
	if (auto &&scriptEngine = scriptLoader.Load("#ScriptEngine"); scriptEngine != nullptr)
	{
		auto scriptEngineModule = scriptEngine->GetTable();

		scriptEngineModule.get<sol::protected_function>("initialize")(_lua.globals());

		_luaLockMetatable = scriptEngineModule.get<sol::protected_function>("lockMetatable");
		_luaPostProcessModGlobals = scriptEngineModule.get<sol::protected_function>("postProcessModGlobals");
		_luaPostProcessScriptGlobals = scriptEngineModule.get<sol::protected_function>("postProcessScriptGlobals");

		AssertLuaValue(_luaLockMetatable, "#ScriptEngine.lockMetatable");
		AssertLuaValue(_luaPostProcessModGlobals, "#ScriptEngine.postProcessModGlobals");
		AssertLuaValue(_luaPostProcessScriptGlobals, "#ScriptEngine.postProcessScriptGlobals");
	}
	else
	{
		Fatal("Could not find core lua module \"#ScriptEngine\"!");
	}

	for (std::string_view key : preRequiredStdModules)
	{
		_lua[key] = _luaSTDRequire(key);
	}

	// for (const char *scriptName : preFullyLoadScriptNames)
	// {
	// 	scriptLoader.Load(scriptName)->FullLoad(scriptLoader);
	// }

	MakeReadonlyGlobal(_lua.globals());
}

void ScriptEngine::AssertLuaValue(sol::object value, std::string_view name) noexcept
{
	if (!value.valid()) [[unlikely]]
	{
		Fatal("Could not find valid core lua module value \"{}\"", name);
	}
}

void ScriptEngine::Deinitialize() noexcept
{
	_persistVariables.clear();
	CollectGarbage();
}

sol::state_view &ScriptEngine::GetState()
{
	return _lua;
}

void ScriptEngine::SetReadonlyGlobal(const std::string_view &key, sol::object value)
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

void ScriptEngine::SetMetatable(sol::table tbl, sol::metatable mt)
{
	sol::protected_function_result result = _lua.globals()["setmetatable"](tbl, mt);
	if (!result.valid()) [[unlikely]]
	{
		sol::error err = result;
		throw err;
	}
}

void ScriptEngine::RawSet(sol::table tbl, sol::object key, sol::object value)
{
	sol::protected_function_result result = _lua.globals()["rawset"](tbl, key, value);
	if (!result.valid()) [[unlikely]]
	{
		sol::error err = result;
		throw err;
	}
}

size_t ScriptEngine::GetMemory() const noexcept
{
	return _lua.memory_used();
}

sol::object ScriptEngine::LuaRequire(sol::string_view targetScriptName, ScriptRequire *script) noexcept
{
	IScriptProvider &scriptProvider = GetScriptProvider();

	ScriptID targetScriptID = scriptProvider.GetScriptIDByName(targetScriptName);
	if (targetScriptID == 0 && script != nullptr)
	{
		// Require script by relative location.
		auto scriptName = scriptProvider.GetScriptNameByID(script->id);
		if (scriptName.has_value())
		{
			auto str = std::string(scriptName.value());
			std::string relativeScriptName = str.replace(str.rfind('.') + 1, std::string::npos, targetScriptName);
			targetScriptID = scriptProvider.GetScriptIDByName(relativeScriptName);
		}
	}
	if (targetScriptID == 0)
	{
		// Require static script. Sandboxed script has no permission to access permissive core modules.
		std::string staticScriptName = std::format("#{}", targetScriptName);
		if (!(script != nullptr && script->sandboxed && permissiveModules.contains(staticScriptName)))
		{
			targetScriptID = scriptProvider.GetScriptIDByName(staticScriptName);
		}
	}

	if (targetScriptID != 0)
	{
		IScriptLoader &scriptLoader = GetScriptLoader();

		if (script != nullptr)
		{
			if (!scriptLoader.IsScriptValid(script->id)) [[unlikely]]
			{
				GetScriptEngine().ThrowError("Cannot require '{}': Target module is invalid (probably loadtime error)", targetScriptName.data());
			}

			if (!scriptProvider.IsStaticScript(targetScriptID))
			{
				scriptLoader.AddReverseDependency(script->id, targetScriptID);
			}
		}

		const std::shared_ptr<IScriptModule> &targetModule = scriptLoader.Load(targetScriptID);
		if (targetModule)
		{
			if (scriptProvider.IsStaticScript(targetScriptID))
			{
				return targetModule->RawLoad();
			}
			else
			{
				return targetModule->LazyLoad();
			}
		}
	}

	if (script != nullptr)
	{
		const auto &virtualModule = script->globals[targetScriptName];
		if (virtualModule.valid() && std::find(preRequiredStdModules.begin(), preRequiredStdModules.end(), targetScriptName) != preRequiredStdModules.end())
		{
			return virtualModule;
		}
	}

	// ! CALL DECONSTRUCTOR ON LUA LONGJMP
	std::string errorMessage;

	{
		sol::protected_function_result result = _luaSTDRequire(targetScriptName);
		if (result.valid()) [[likely]]
		{
			return result;
		}
		else [[unlikely]]
		{
			sol::error err = result;
			errorMessage = err.what();
		}
	}

	GetScriptEngine().ThrowError(errorMessage);
	return {};
}

sol::table ScriptEngine::CreateTable(uint32_t arr, uint32_t hash) noexcept
{
	return _lua.create_table(arr, hash);
}

std::string ScriptEngine::DebugTraceback(std::string_view message, std::double_t level) noexcept
{
	sol::protected_function_result result = _luaStackTrackPlus["stacktrace"](message, level);
	TE_ASSERT(result.valid());
	return std::string(result.get<sol::string_view>());
}

sol::load_result ScriptEngine::LoadFunction(std::string_view name, std::string_view code)
{
	return _lua.load(code, std::string(name), sol::load_mode::any);
}

std::string ScriptEngine::Inspect(sol::object obj)
{
	if (!obj.valid()) [[unlikely]]
	{
		throw sol::error("Must be a valid lua value!");
	}

	sol::protected_function_result result = _luaInspect(obj);
	if (!result.valid()) [[unlikely]]
	{
		sol::error err = result;
		TE_FATAL("Failed to inspect table: {}", err.what());
	}

	return std::string(result.get<sol::string_view>());
}

void ScriptEngine::ThrowError(std::string &message) noexcept
{
	lua_pushlstring(_lua, message.data(), message.size());
	LuaUtils::Deconstruct(message);
	lua_error(_lua);
}

sol::object ScriptEngine::MakeReadonlyGlobalImpl(sol::object obj, std::unordered_map<sol::table, sol::table, LuaTableHash, LuaTableEqual> &visited) noexcept
{
	if (obj.get_type() != sol::type::table)
	{
		return obj;
	}

	auto &&table = obj.as<sol::table>();

	auto it = visited.find(table);
	if (it != visited.end())
	{
		return it->second;
	}

	if (table[sol::metatable_key].valid())
	{
		return obj;
	}

	sol::table &&proxy = _lua.create_table();
	sol::table &&metatable = _lua.create_table(0, 2);

	proxy[sol::metatable_key] = metatable;
	visited[table] = proxy;

	for (auto &&[key, value] : table)
	{
		table[key] = MakeReadonlyGlobalImpl(value, visited);
	}

	metatable["__index"] = table;
	metatable["__newindex"] = _luaThrowModifyReadonlyGlobalError;

	sol::protected_function_result result = _luaLockMetatable(metatable);
	if (!result.valid()) [[unlikely]]
	{
		sol::error error = result;
		TE_FATAL("Error while mark locked table: {}", error.what());
	}

	return proxy;
}

sol::object ScriptEngine::MakeReadonlyGlobal(sol::object obj)
{
	Initialize();
	std::unordered_map<sol::table, sol::table, LuaTableHash, LuaTableEqual> visited{};
	return MakeReadonlyGlobalImpl(obj, visited);
}

sol::table &ScriptEngine::GetModGlobals(std::string_view modUID, bool sandboxed) noexcept
{
	if (auto it = _modsGlobals.find(modUID); it != _modsGlobals.end())
	{
		return it->second;
	}

	auto &luaGlobals = _lua.globals();
	auto modGlobals = CreateTable();

	for (std::string_view key : GetLuaBindings().GetModGlobalsMigration())
	{
		TE_ASSERT(!modGlobals[key.data()].valid(), "global field duplicated!");
		modGlobals[key.data()] = luaGlobals[key.data()];
	}

	sol::table metatable = _lua.create_table();
	metatable["__index"] = [this](const sol::this_state &ts, sol::object, sol::object key)
	{
		auto scriptName = GetScriptLoader().GetLoadingScriptName().value_or("");

		EventHandleKey eventKey;
		if (key.is<sol::string_view>())
		{
			eventKey = EventHandleKey(key.as<std::string_view>());
		}
		else if (key.is<std::double_t>())
		{
			eventKey = EventHandleKey(key.as<std::double_t>());
		}

		EventModGlobalsIndexData eventData{
		    .scriptName = scriptName,
		    .key = key,
		    .value = sol::nil,
		};
		GetEventManager().GetCoreEvents().ScriptGlobalIndex().Invoke(&eventData, eventKey);
		return eventData.value;
	};
	modGlobals[sol::metatable_key] = metatable;
	_luaLockMetatable(metatable);

	modGlobals["_G"] = modGlobals;

	{
		sol::protected_function_result postProcessResult = _luaPostProcessModGlobals(modUID, sandboxed, modGlobals, luaGlobals);
		if (!postProcessResult.valid()) [[unlikely]]
		{
			sol::error error = postProcessResult;
			TE_FATAL("Error while post process mod globals: {}", error.what());
		}
	}

	auto result = _modsGlobals.try_emplace(modUID, modGlobals);
	TE_ASSERT(result.second);
	return result.first->second;
}

void ScriptEngine::InitializeScript(ScriptID scriptID, std::string_view scriptName, std::string_view modUID, bool sandboxed, sol::protected_function &func) noexcept
{
	sol::table &modGlobals = GetModGlobals(modUID, sandboxed);
	sol::environment scriptGlobals{_lua, sol::create, modGlobals};

	if (!scriptGlobals.valid()) [[unlikely]]
	{
		_log->Error("Failed to create script globals!");
		return;
	}

	scriptGlobals["log"] = Log::Get(scriptName);

	scriptGlobals.set_function("persist", [this, scriptName](sol::string_view key, sol::object defaultValue, sol::object getter)
	{
		if (getter.is<sol::protected_function>())
		{
			if (defaultValue == sol::nil) [[unlikely]]
			{
				GetScriptEngine().ThrowError("Default value could not be nil");
			}

			return RegisterPersistVariable(scriptName, key, defaultValue, getter.as<sol::protected_function>());
		}
		else if (defaultValue.is<sol::protected_function>())
		{
			sol::protected_function getter_ = defaultValue.as<sol::protected_function>();
			defaultValue = getter_();

			return RegisterPersistVariable(scriptName, key, defaultValue, getter_);
		}
		else [[unlikely]]
		{
			GetScriptEngine().ThrowError("Invalid function call");
		}
	});

	scriptGlobals.set_function("print", [this, scriptName](const sol::variadic_args &args)
	{
		const std::shared_ptr<Log> &log = Log::Get(scriptName);
		if (!log->CanDebug())
		{
			return;
		}

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
	});

	scriptGlobals.set_function("require", [this, scriptGlobals, scriptID, sandboxed](sol::string_view targetScriptName) -> sol::object
	{
		ScriptRequire script{scriptID, scriptGlobals, sandboxed};
		return LuaRequire(targetScriptName, &script);
	});

	sol::table metatable = _lua.create_table(0, 1);
	metatable["__index"] = modGlobals;
	scriptGlobals[sol::metatable_key] = metatable;

	scriptGlobals["_G"] = scriptGlobals;

	sol::set_environment(scriptGlobals, func);

	sol::protected_function_result result = _luaPostProcessScriptGlobals(scriptID, scriptName, modUID, sandboxed, func, scriptGlobals, _lua.globals());
	if (!result.valid()) [[unlikely]]
	{
		sol::error error = result;
		TE_FATAL("Error while post process script globals: {}", error.what());
	}
}

void ScriptEngine::DeinitializeScript(ScriptID scriptID, std::string_view scriptName)
{
	SaveScriptPersistVariables(scriptName);
}

void ScriptEngine::SaveScriptPersistVariables(std::string_view scriptName) noexcept
{
	auto it = _persistVariables.find(scriptName.data());
	if (it == _persistVariables.end())
	{
		return;
	}

	for (auto &&[_, variable] : it->second)
	{
		variable.Save();
	}
}

void ScriptEngine::SavePersistVariables() noexcept
{
	for (auto &&[scriptName, variables] : _persistVariables)
	{
		for (auto &&[key, variable] : variables)
		{
			variable.Save();
		}
	}
}

sol::object ScriptEngine::RegisterPersistVariable(std::string_view scriptName, std::string_view key, sol::object defaultValue, const sol::protected_function &getter)
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

std::unordered_map<std::string_view, sol::object> ScriptEngine::GetScriptPersistVariables(std::string_view scriptName) noexcept
{
	auto it = _persistVariables.find(scriptName.data());
	if (it == _persistVariables.end())
	{
		return {};
	}

	std::unordered_map<std::string_view, sol::object> variables{};

	for (const auto &[key, variable] : it->second)
	{
		variables.emplace(key, variable.value);
	}

	return variables;
}

bool ScriptEngine::ClearScriptPersistVariables(std::string_view scriptName) noexcept
{
	return _persistVariables.erase(scriptName.data());
}

void ScriptEngine::ClearPersistVariables() noexcept
{
	_persistVariables.clear();
}
