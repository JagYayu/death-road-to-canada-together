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
#include "sol/optional_implementation.hpp"
#include "sol/protected_function_result.hpp"
#include "sol/types.hpp"
#include "sol/variadic_args.hpp"
#include "system/LogMicros.hpp"
#include "util/Utils.hpp"
#include <exception>
#include <memory>
#include <stdexcept>

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
	TE_ASSERT(_parentContext != nullptr);
	return *_parentContext;
}

Log &ScriptModule::GetLog() noexcept
{
	TE_ASSERT(!_parentLog.expired());
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

void CopyTableMetatableFields(sol::table dst, sol::table src)
{
	if (!dst.valid() || !src.valid())
	{
		return;
	}

	if (!src[sol::metatable_key].valid())
	{
		return;
	}

	sol::table dstMt = dst[sol::metatable_key];
	sol::table srcMt = src[sol::metatable_key];

	if (!dstMt.valid() || !srcMt.valid())
	{
		return;
	}

	constexpr const char *keys[] = {
	    "__call",
	};

	for (const char *key : keys)
	{
		sol::object field = srcMt[key];
		if (field.valid() && field.is<sol::protected_function>())
		{
			auto f = field.as<sol::protected_function>();
			dstMt[key] = [f, src](const sol::this_state &ts, sol::object, sol::variadic_args vargs)
			{
				return f(src, vargs);
			};
		}
	}
}

sol::table &ScriptModule::RawLoad(IScriptLoader &scriptLoader)
{
	if (_fullyLoaded)
	{
		return _table;
	}

	TE_ASSERT(_scriptID);

	ScriptLoader &parent = static_cast<ScriptLoader &>(scriptLoader);
	_parentContext = &parent._context;
	_parentLog = parent._log;

	if (!_func.valid()) [[unlikely]]
	{
		_table = GetScriptEngine().CreateTable();
		sol::table metatable = GetScriptEngine().CreateTable(0, 2);
		// metatable["__index"] = []() {};
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
	{
		auto tblRes = result.get<sol::object>(0);
		if (tblRes.get_type() == sol::type::table)
		{
			tbl = tblRes;
		}
		else
		{
			tbl = scriptLoader.GetScriptEngine().CreateTable();
		}
	}

	IScriptEngine &scriptEngine = GetScriptEngine();
	_table = scriptEngine.CreateTable();
	sol::metatable metatable = scriptEngine.CreateTable(0, 2);
	_table[sol::metatable_key] = metatable;

	CopyTableMetatableFields(_table, tbl);
	metatable["__index"] = tbl;
	metatable["__newindex"] = [this](const sol::this_state &ts)
	{
		GetScriptEngine().ThrowError("Cannot override module");
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
	metatable["__newindex"] = [this](const sol::this_state &ts)
	{
		GetScriptEngine().ThrowError("Cannot override module");
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

	TE_ASSERT(_scriptID);

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

			GetScriptEngine().ThrowError("Cyclic dependency detected between <{}>\"{}\" and <{}>\"{}\"", _scriptID, *optScriptName, *optPrevScriptID, *optPrevScriptName);
		}
		else
		{
			GetScriptEngine().ThrowError("Attempt to access incomplete module");
		}
	};
	metatable["__newindex"] = [this, &parent](const sol::this_state &ts, sol::object, sol::object key, sol::object value)
	{
		throw std::runtime_error(std::format("Cannot override module"));
	};

	parent._scriptLoopLoadStack.emplace_back(_scriptID);
	sol::protected_function_result result = _func();
	TE_ASSERT(parent._scriptLoopLoadStack.back() == _scriptID);
	parent._scriptLoopLoadStack.pop_back();

	sol::table tbl;
	if (!result.valid()) [[unlikely]]
	{
		std::string_view scriptName = GetScriptProvider().GetScriptNameByID(_scriptID).value_or("");
		sol::error err = result;
		auto message = err.what();

		TE_ERROR("Error load script module <{}>\"{}\": {}", _scriptID, scriptName, message);

		// TODO use delegate event?
		GetScriptErrors().AddLoadtimeError(_scriptID, message);

		_hasError = true;

		tbl = scriptEngine.CreateTable();
		sol::table metatable = scriptEngine.CreateTable();
		metatable["__index"] = [this, scriptName](const sol::this_state &ts, sol::object, sol::object key)
		{
			if (_parentContext) [[likely]]
			{
				GetScriptEngine().ThrowError("Cascaded error occurred: <{}>\"{}\"", _scriptID, scriptName);
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

		auto tblRes = result.get<sol::object>(0);
		if (tblRes.get_type() == sol::type::table)
		{
			tbl = tblRes.as<sol::table>();
		}
		else
		{
			if (tblRes.get_type() != sol::type::nil)
			{
				TE_WARN("'{}': Does not support receiving non-table values", parent.GetLoadingScriptName().value());
			}
			tbl = scriptEngine.CreateTable();
		}
	}

	CopyTableMetatableFields(_table, tbl);
	metatable["__index"] = tbl;

	parent._loadingScript = previousScriptID;
	_fullyLoaded = true;
	engine.AddLoadingProgress(1);

	return _table;
}
