/**
 * @file Mod/ScriptModule.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Mod/ScriptModule.hpp"

#include "Mod/ScriptEngine.hpp"
#include "Mod/ScriptErrors.hpp"
#include "Mod/ScriptLoader.hpp"
#include "Mod/ScriptProvider.hpp"
#include "Program/Engine.hpp"
#include "System/LogMicros.hpp"
#include "Util/LuaUtils.hpp"
#include "Util/Utils.hpp"

#include "sol/forward.hpp"
#include "sol/protected_function_result.hpp"
#include "sol/types.hpp"
#include "sol/variadic_args.hpp"

#include <memory>
#include <stdexcept>

using namespace tudov;

Context *ScriptModule::_parentContext = nullptr;
std::weak_ptr<Log> ScriptModule::_parentLog = std::shared_ptr<Log>(nullptr);

ScriptModule::ScriptModule(IScriptLoader &scriptLoader) noexcept
    : _scriptLoader(scriptLoader),
      _scriptID(0),
      _func(),
      _fullyLoaded(false),
      _hasError(false),
      _table()
{
}

ScriptModule::ScriptModule(IScriptLoader &scriptLoader, ScriptID scriptID, const sol::protected_function &func) noexcept
    : _scriptLoader(scriptLoader),
      _scriptID(scriptID),
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

IScriptLoader &ScriptModule::GetScriptLoader() noexcept
{
	return _scriptLoader;
}

bool ScriptModule::IsValid() const noexcept
{
	return _func.valid();
}

bool ScriptModule::IsLazyLoaded() const noexcept
{
	return _table.valid();
}

bool ScriptModule::IsFullyLoaded() const noexcept
{
	return _fullyLoaded;
}

bool ScriptModule::HasLoadError() const noexcept
{
	return _fullyLoaded && _hasError;
}

void ScriptModule::MarkLoadError() noexcept
{
	_hasError = true;
}

sol::table &ScriptModule::GetTable()
{
	return _table;
}

/**
 * Copy some common metatable fields to mimic the behavior of the original returned table.
 */
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
	    "__add",
	    "__call",
	    "__concat",
	    "__div",
	    "__eq",
	    "__le",
	    "__lt",
	    "__mod",
	    "__mul",
	    "__sub",
	    "__tostring",
	    "__unm",
	};

	for (const char *key : keys)
	{
		sol::object field = srcMt[key];
		if (field.valid() && field.is<sol::protected_function>())
		{
			auto f = field.as<sol::protected_function>();
			dstMt[key] = [f, src](const sol::this_state &, sol::object, sol::variadic_args vargs)
			{
				return f(src, vargs);
			};
		}
	}
}

void ScriptModule::ModuleFieldModifier(std::shared_ptr<ScriptModule> module, sol::object key, sol::object value) noexcept
{
	if (module == nullptr) [[unlikely]]
	{
		if (module->CanWarn())
		{
			module->Warn("Attempt to modify member from a deallocated module!");
		}

		return;
	}

	if (module->GetScriptLoader().GetLoadingScriptID() == 0) [[unlikely]]
	{
		module->GetScriptEngine().ThrowError("Cannot modify module field at script runtime!");
	}

	auto &&field = module->GetTable()[key];
	if (!field.valid() || field.get_type() == sol::type::nil) [[unlikely]]
	{
		module->GetScriptEngine().ThrowError("Module field does not exists");
	}

	if (field.get_type() != value.get_type()) [[unlikely]]
	{
		module->GetScriptEngine().ThrowError("Module field's type mismatch value's type");
	}

	module->GetTable()[key] = value;
}

sol::table &ScriptModule::RawLoad()
{
	if (_fullyLoaded)
	{
		return _table;
	}

	TE_ASSERT(_scriptID);

	ScriptLoader &parent = static_cast<ScriptLoader &>(_scriptLoader);
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

	engine.SetLoadingDescription(_scriptLoader.GetScriptProvider().GetScriptNameByID(_scriptID).value());

	ScriptID previousLoadingScript = parent._loadingScript;
	parent._loadingScript = _scriptID;
	sol::protected_function_result result = _func();
	if (!result.valid()) [[unlikely]]
	{
		sol::error err = result;
		Fatal("'{}': {}", _scriptLoader.GetLoadingScriptName().value(), err.what());
	}

	if (result.get<sol::object>(1))
	{
		TE_WARN("'{}': Does not support receiving multiple return values", _scriptLoader.GetLoadingScriptName().value());
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
			tbl = _scriptLoader.GetScriptEngine().CreateTable();
		}
	}

	IScriptEngine &scriptEngine = GetScriptEngine();
	_table = scriptEngine.CreateTable();
	sol::metatable metatable = scriptEngine.CreateTable(0, 2);
	_table[sol::metatable_key] = metatable;

	CopyTableMetatableFields(_table, tbl);

	std::weak_ptr<ScriptModule> weakThis = shared_from_this();
	metatable["__index"] = tbl;
	metatable["__newindex"] = [weakThis](const sol::this_state &, sol::object, const sol::object key, const sol::object value)
	{
		ModuleFieldModifier(weakThis.lock(), key, value);
	};

	_fullyLoaded = true;
	engine.AddLoadingProgress(1);

	return _table;
}

sol::table &ScriptModule::LazyLoad()
{
	if (!_table.valid())
	{
		_table = _scriptLoader.GetScriptEngine().CreateTable();
	}

	if (_table[sol::metatable_key].valid())
	{
		return _table;
	}

	if (!_func.valid()) [[unlikely]]
	{
		_table = _scriptLoader.GetScriptEngine().CreateTable();
		_fullyLoaded = true;
		return _table;
	}

	sol::table metatable = _scriptLoader.GetScriptEngine().CreateTable();

	_table[sol::metatable_key] = metatable;

	std::weak_ptr<ScriptModule> weakThis = shared_from_this();
	metatable["__index"] = [weakThis](const sol::this_state &, sol::object, sol::object key) -> sol::object
	{
		if (std::shared_ptr<ScriptModule> this_ = weakThis.lock(); this_ != nullptr) [[likely]]
		{
			return this_->FullLoad()[key];
		}
		else if (auto engine = Tudov::GetApplication<Engine>(); engine != nullptr)
		{
			auto &scriptEngine = engine->GetContext().GetScriptEngine();
			LuaUtils::Deconstruct(engine);
			scriptEngine.ThrowError("Attempt to access member from a deallocated module!");
		}
		else
		{
			return sol::nil;
		}
	};
	metatable["__newindex"] = [weakThis](const sol::this_state &, sol::object, sol::object key, sol::object value)
	{
		ModuleFieldModifier(weakThis.lock(), key, value);
	};

	return _table;
}

sol::table &ScriptModule::FullLoad()
{
	if (_fullyLoaded)
	{
		return _table;
	}

	if (!_func.valid()) [[unlikely]]
	{
		_table = _scriptLoader.GetScriptEngine().CreateTable();
		_fullyLoaded = true;
		return _table;
	}

	TE_ASSERT(_scriptID);

	auto &parent = static_cast<ScriptLoader &>(_scriptLoader);
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

	std::weak_ptr<ScriptModule> weakThis = shared_from_this();
	metatable["__index"] = [this, &parent](const sol::this_state &, sol::object, sol::object key)
	{
		if (const auto &optPrevScriptID = FindPreviousInStack(parent._scriptLoopLoadStack, _scriptID); optPrevScriptID.has_value())
		{
			IScriptProvider &scriptProvider = GetScriptProvider();
			auto optScriptName = scriptProvider.GetScriptNameByID(_scriptID);
			auto optPrevScriptName = scriptProvider.GetScriptNameByID(*optPrevScriptID);

			GetScriptEngine().ThrowError("Cyclic dependency detected between <{}>{} and <{}>{}", _scriptID, *optScriptName, *optPrevScriptID, *optPrevScriptName);
		}
		else
		{
			GetScriptEngine().ThrowError("Attempt to access incomplete module");
		}
	};
	metatable["__newindex"] = [weakThis](const sol::this_state &, sol::object, sol::object key, sol::object value)
	{
		ModuleFieldModifier(weakThis.lock(), key, value);
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

		TE_ERROR("Error load script module <{}>{}: {}", _scriptID, scriptName, message);

		// TODO use delegate event?
		GetScriptErrors().AddLoadtimeError(_scriptID, message);

		_hasError = true;

		tbl = scriptEngine.CreateTable();
		sol::table metatable = scriptEngine.CreateTable();
		metatable["__index"] = [this, scriptName](const sol::this_state &, sol::object, sol::object key)
		{
			if (_parentContext) [[likely]]
			{
				GetScriptEngine().ThrowError("Cascaded error occurred: <{}>{}", _scriptID, scriptName);
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
