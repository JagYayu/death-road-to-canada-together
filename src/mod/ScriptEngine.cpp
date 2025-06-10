#include "ScriptEngine.h"

#include "ModManager.h"
#include "ScriptLoader.h"
#include "program/Engine.h"
#include "sol/variadic_args.hpp"
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

#include <optional>
#include <unordered_map>

using namespace tudov;

ScriptEngine::ScriptEngine(ModManager &modManager)
    : modManager(modManager),
      scriptLoader(*this),
      _log(Log::Get("ScriptEngine")),
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

	_error = _lua["error"];
	_traceback = _lua["debug"]["traceback"];
	_throwModifyReadonlyGlobalError = _lua.load("error('Attempt to modify read-only global', 2)").get<sol::function>();

	std::unordered_map<sol::table, sol::table, LuaTableHash, LuaTableEqual> visited;
	MakeReadonlyGlobal(_lua.globals(), visited);
}

ScriptEngine::~ScriptEngine()
{
	_persistVariables.clear();
	_virtualGlobal = sol::nil;
	_error = sol::nil;
	_traceback = sol::nil;
	_lua.collect_garbage();
}

sol::object ScriptEngine::MakeReadonlyGlobal(const sol::object &obj, std::unordered_map<sol::table, sol::table, LuaTableHash, LuaTableEqual> &visited)
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

	sol::table proxy = _lua.create_table();
	sol::table metatable = _lua.create_table(0, 2);

	proxy[sol::metatable_key] = metatable;
	visited[table] = proxy;

	for (auto &&[key, value] : table)
	{
		table[key] = MakeReadonlyGlobal(value, visited);
	}

	metatable["__index"] = table;
	metatable["__newindex"] = _throwModifyReadonlyGlobalError;

	return proxy;
}

sol::state_view &ScriptEngine::GetState()
{
	return _lua;
}

sol::table ScriptEngine::CreateTable(uint32_t arr, uint32_t hash)
{
	return _lua.create_table(arr, hash);
}
sol::load_result ScriptEngine::LoadFunction(const String &name, const StringView &code)
{
	return _lua.load(code, name, sol::load_mode::any);
}

void ScriptEngine::ThrowError(const StringView &message, uint32_t level)
{
}

// void ScriptEngine::Require(const StringView &source, const StringView &target)
// {
// 	throw std::runtime_error("Permission denied");
// }

void ScriptEngine::OnFatalException(const std::exception &e)
{
	_log->Fatal(Format("Unhandled C++ exception occurred: {}", e.what()));
}

void ScriptEngine::InitScriptFunc(const String &scriptName, sol::protected_function &func)
{
	auto &&namespace_ = GetLuaNamespace(scriptName);

	sol::environment env{_lua, sol::create, _lua.globals()};

	auto &&log = Log::Get(scriptName);

	env["print"] = [&, log](sol::variadic_args args)
	{
		try
		{
			for (auto &&arg : args)
			{
				if (arg.is<sol::string_view>())
				{
					auto &&what = arg.as<sol::string_view>();
					log->Debug(String(what));
				}
				else if (arg.is<int>())
				{
					log->Debug("// TODO");
				}
				else
				{
					log->Debug("// TODO");
				}
			}
		}
		catch (const std::exception &e)
		{
			OnFatalException(e);
			throw;
		}
	};

	env["require"] = [&, log](const String &scriptName) -> sol::object
	{
		try
		{
			auto &&scriptModule = scriptLoader.Load(scriptName);
			if (!scriptModule.has_value())
			{
				luaL_error(_lua, "Script module '%s' not found", scriptName.c_str());
				return sol::nil;
			}

			return scriptModule.value().get().LazyLoad(scriptLoader);
		}
		catch (const std::exception &e)
		{
			OnFatalException(e);
			throw;
		}
	};

	env["Event_add"] = [&, log](const sol::table &args)
	{
		try
		{
			auto &&loadingScript = scriptLoader.GetLoadingScript();
			if (!loadingScript.has_value())
			{
				_error("Failed to add event handler: must add at script load time");
				return;
			}

			auto &&argName = args["name"];
			if (!argName.valid() || !argName.is<sol::string_view>())
			{
				_error(Format("Failed to add event handler: invalid `name` type, expected string, got {}", GetLuaTypeStringView(argName.get_type())));
				return;
			}
			auto &&argFunc = args["func"];
			if (!argFunc.valid() || !argFunc.is<sol::function>())
			{
				_error(Format("Failed to add event handler: invalid `func` type, expected function, got {}", GetLuaTypeStringView(argFunc.get_type())));
				return;
			}

			auto &&name = argName.get<sol::string_view>();
			auto &&event = modManager.engine.eventManager.TryGetRegistryEvent(name);
			if (!event.has_value())
			{
				_error(Format("Failed to add event handler: event named '{}' not found", name));
				return;
			}

			auto &&argOrder = args["order"];
			Optional<String> order;
			if (!argOrder.valid() || argOrder.is<sol::nil_t>())
			{
				order = null;
			}
			else if (argOrder.is<sol::string_view>())
			{
				order = argOrder.get<sol::string_view>();
			}
			else
			{
				_error(Format("Failed to add event handler: invalid `order` type, nil or string expected, got {}", GetLuaTypeStringView(argOrder.get_type())));
				return;
			}

			auto &&argKey = args["key"];
			Optional<AddHandlerArgs::Key> key;
			if (!argKey.valid() || argKey.is<sol::nil_t>())
			{
				key = null;
			}
			else if (argKey.is<Number>())
			{
				key = AddHandlerArgs::Key(argKey.get<Number>());
			}
			else if (argKey.is<sol::string_view>())
			{
				key = AddHandlerArgs::Key(String(argKey.get<sol::string_view>()));
			}
			else
			{
				_error(Format("Failed to add event handler: invalid `key` type, nil or number or string expected, got {}", GetLuaTypeStringView(argKey.get_type())));
				return;
			}

			auto &&argSequence = args["sequence"];
			Optional<Number> sequence;
			if (!argSequence.valid() || argSequence.is<sol::nil_t>())
			{
				sequence = null;
			}
			else if (argSequence.is<Number>())
			{
				sequence = argSequence.get<Number>();
			}
			else
			{
				_error(Format("Failed to add event handler: invalid `sequence` type, nil or string expected, got {}", GetLuaTypeStringView(argOrder.get_type())));
				return;
			}

			event->get().Add(AddHandlerArgs{
			    .scriptName = loadingScript.value(),
			    .name = String(name),
			    .function = {argName.get<sol::function>()},
			    .order = Move(order),
			    .key = Move(key),
			    .sequence = Move(sequence),
			});
		}
		catch (const std::exception &e)
		{
			OnFatalException(e);
			throw;
		}
	};

	sol::set_environment(env, func);
}

sol::object ScriptEngine::GetPersistVariable(const StringView &key)
{
	auto &&it = _persistVariables.find(key);
	if (it == _persistVariables.end())
	{
		return sol::nil;
	}
	return it->second;
}

void ScriptEngine::SetPersistVariable(const StringView &key, const sol::object &value)
{
	_persistVariables[key] = value;
}
