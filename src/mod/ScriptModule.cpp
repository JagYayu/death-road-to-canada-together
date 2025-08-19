/**
 * @file mod/ScriptModule.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "mod/ScriptModule.hpp"

#include "mod/ScriptEngine.hpp"
#include "mod/ScriptErrors.hpp"
#include "mod/ScriptLoader.hpp"
#include "mod/ScriptProvider.hpp"
#include "program/Engine.hpp"
#include "sol/forward.hpp"
#include "sol/types.hpp"
#include "util/LogMicros.hpp"
#include "util/Utils.hpp"

using namespace tudov;

Context *ScriptModule::_parentContext = nullptr;
std::weak_ptr<Log> ScriptModule::_parentLog = std::shared_ptr<Log>(nullptr);

ScriptModule::ScriptModule()
    : _scriptID(0),
      _func(),
      _fullyLoaded(false),
      _hasError(false),
      _table()
{
}

ScriptModule::ScriptModule(ScriptID scriptID, const sol::protected_function &func)
    : _scriptID(scriptID),
      _func(func),
      _fullyLoaded(false),
      _hasError(false),
      _table()
{
}

Context &ScriptModule::GetContext() noexcept
{
	assert(_parentContext != nullptr);
	return *_parentContext;
}

Log &ScriptModule::GetLog() noexcept
{
	assert(!_parentLog.expired());
	return *_parentLog.lock();
}

bool ScriptModule::IsValid() const
{
	return _func.valid();
}

bool ScriptModule::IsLazyLoaded() const
{
	return _table.valid();
}

bool ScriptModule::IsFullyLoaded() const
{
	return _fullyLoaded;
}

bool ScriptModule::HasLoadError() const
{
	return _fullyLoaded && _hasError;
}

sol::table &ScriptModule::GetTable()
{
	return _table;
}

sol::table &ScriptModule::RawLoad(IScriptLoader &scriptLoader)
{
	if (_fullyLoaded)
	{
		return _table;
	}

	assert(_scriptID);

	ScriptLoader &parent = static_cast<ScriptLoader &>(scriptLoader);
	_parentContext = &parent._context;
	_parentLog = parent._log;

	if (!_func.valid()) [[unlikely]]
	{
		_table = GetScriptEngine().CreateTable();
		sol::table metatable = GetScriptEngine().CreateTable(0, 2);
		metatable["__index"] = []() {};
		_table[sol::metatable_key] = metatable;

		_fullyLoaded = true;
		return _table;
	}

	Engine &engine = GetEngine();

	engine.SetLoadingDescription(scriptLoader.GetScriptProvider().GetScriptNameByID(_scriptID).value());

	ScriptID previousLoadingScript = parent._loadingScript;
	parent._loadingScript = _scriptID;
	sol::protected_function_result result = _func();
	if (!result.valid()) [[unlikely]]
	{
		sol::error err = result;
		Fatal("'{}': {}", scriptLoader.GetLoadingScriptName().value(), err.what());
	}

	if (result.get<sol::object>(1))
	{
		TE_WARN("'{}': Does not support receiving multiple return values", scriptLoader.GetLoadingScriptName().value());
	}

	parent._loadingScript = previousLoadingScript;
	sol::table tbl;
	if (result.get<sol::object>(0).get_type() == sol::type::table)
	{
		tbl = result.get<sol::object>(0);
	}
	else
	{
		tbl = scriptLoader.GetScriptEngine().CreateTable();
	}

	IScriptEngine &scriptEngine = GetScriptEngine();
	_table = scriptEngine.CreateTable();
	sol::table metatable = scriptEngine.CreateTable(0, 2);
	_table[sol::metatable_key] = metatable;

	metatable["__index"] = tbl;
	metatable["__newindex"] = [this, &parent](const sol::this_state &ts, sol::object, sol::object key, sol::object value)
	{
		parent.GetScriptEngine().ThrowError(std::format("Cannot override module"));
	};

	_fullyLoaded = true;
	engine.AddLoadingProgress(1);

	return _table;
}

sol::table &ScriptModule::LazyLoad(IScriptLoader &scriptLoader)
{
	if (!_table.valid())
	{
		_table = scriptLoader.GetScriptEngine().CreateTable();
	}

	if (_table[sol::metatable_key].valid())
	{
		return _table;
	}

	if (!_func.valid()) [[unlikely]]
	{
		_table = scriptLoader.GetScriptEngine().CreateTable();
		_fullyLoaded = true;
		return _table;
	}

	sol::table metatable = scriptLoader.GetScriptEngine().CreateTable();

	_table[sol::metatable_key] = metatable;

	metatable["__index"] = [this, &scriptLoader](const sol::this_state &ts, sol::object, sol::object key)
	{
		return FullLoad(scriptLoader)[key];
	};
	metatable["__newindex"] = [this, &scriptLoader](const sol::this_state &ts, sol::object, sol::object key, sol::object value)
	{
		scriptLoader.GetScriptEngine().ThrowError(std::format("Cannot override module"));
	};

	return _table;
}

sol::table &ScriptModule::FullLoad(IScriptLoader &scriptLoader)
{
	if (_fullyLoaded)
	{
		return _table;
	}

	if (!_func.valid()) [[unlikely]]
	{
		_table = scriptLoader.GetScriptEngine().CreateTable();
		_fullyLoaded = true;
		return _table;
	}

	assert(_scriptID);

	auto &parent = static_cast<ScriptLoader &>(scriptLoader);
	_parentContext = &parent._context;
	_parentLog = parent._log;

	Engine &engine = GetEngine();

	engine.SetLoadingDescription(GetScriptProvider().GetScriptNameByID(_scriptID).value());

	ScriptID previousScriptID = parent._loadingScript;
	parent._loadingScript = _scriptID;

	IScriptEngine &scriptEngine = parent.GetScriptEngine();

	_table = scriptEngine.CreateTable();
	sol::table metatable = scriptEngine.CreateTable(0, 2);
	_table[sol::metatable_key] = metatable;

	metatable["__index"] = [this, &parent](const sol::this_state &ts, sol::object, sol::object key)
	{
		if (const auto &optPrevScriptID = FindPreviousInStack(parent._scriptLoopLoadStack, _scriptID); optPrevScriptID.has_value())
		{
			IScriptProvider &scriptProvider = GetScriptProvider();
			auto optScriptName = scriptProvider.GetScriptNameByID(_scriptID);
			auto optPrevScriptName = scriptProvider.GetScriptNameByID(*optPrevScriptID);

			parent.GetScriptEngine().ThrowError(std::format("Cyclic dependency detected between <{}>\"{}\" and <{}>\"{}\"", _scriptID, *optScriptName, *optPrevScriptID, *optPrevScriptName));
		}
		else
		{
			parent.GetScriptEngine().ThrowError("Attempt to access incomplete module");
		}
	};
	metatable["__newindex"] = [this, &parent](const sol::this_state &ts, sol::object, sol::object key, sol::object value)
	{
		parent.GetScriptEngine().ThrowError(std::format("Cannot override module"));
	};

	parent._scriptLoopLoadStack.emplace_back(_scriptID);
	sol::protected_function_result result = _func();
	assert(parent._scriptLoopLoadStack.back() == _scriptID);
	parent._scriptLoopLoadStack.pop_back();

	sol::table tbl;
	if (!result.valid()) [[unlikely]]
	{
		sol::error err = result;
		std::string_view what = err.what();

		std::string_view scriptName = GetScriptProvider().GetScriptNameByID(_scriptID).value_or("");
		TE_ERROR("Error load script module <{}>\"{}\": {}", _scriptID, scriptName, what);

		// TODO use delegate event?
		GetScriptErrors().AddLoadtimeError(_scriptID, what);

		_hasError = true;

		tbl = scriptEngine.CreateTable();
		sol::table metatable = scriptEngine.CreateTable();
		metatable["__index"] = [this, scriptName]()
		{
			if (_parentContext) [[likely]]
			{
				_parentContext->GetScriptEngine().ThrowError("Cascaded error occurred: <{}>\"{}\"", _scriptID, scriptName);
			}
		};
		tbl[sol::metatable_key] = metatable;
	}
	else [[likely]]
	{
		if (result.get<sol::object>(1))
		{
			TE_WARN("'{}': Does not support receiving multiple return values", parent.GetLoadingScriptName().value());
		}

		auto value = result.get<sol::object>(0);
		if (value.get_type() == sol::type::table)
		{
			tbl = value.as<sol::table>();
		}
		else
		{
			if (value.get_type() != sol::type::nil)
			{
				TE_WARN("'{}': Does not support receiving non-table values", parent.GetLoadingScriptName().value());
			}
			tbl = scriptEngine.CreateTable();
		}
	}

	metatable["__index"] = tbl;

	parent._loadingScript = previousScriptID;
	_fullyLoaded = true;
	engine.AddLoadingProgress(1);

	return _table;
}
