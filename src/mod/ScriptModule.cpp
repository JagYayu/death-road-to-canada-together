#include "mod/ScriptModule.hpp"

#include "mod/ScriptEngine.hpp"
#include "mod/ScriptErrors.hpp"
#include "mod/ScriptLoader.hpp"
#include "program/Engine.hpp"
#include "util/LogMicros.hpp"

using namespace tudov;

Context *ScriptModule::_parentContext = nullptr;
std::weak_ptr<Log> ScriptModule::_parentLog = std::shared_ptr<Log>(nullptr);

ScriptModule::ScriptModule()
    : _scriptID(),
      _func(),
      _fullyLoaded(),
      _table()
{
}

ScriptModule::ScriptModule(ScriptID scriptID, const sol::protected_function &func)
    : _scriptID(scriptID),
      _func(func),
      _fullyLoaded(),
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

const sol::table &ScriptModule::GetTable()
{
	return _table;
}

const sol::table &ScriptModule::RawLoad(IScriptLoader &scriptLoader)
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

	auto &&parent = static_cast<ScriptLoader &>(scriptLoader);
	_parentContext = &parent._context;
	_parentLog = parent._log;

	auto &&engine = scriptLoader.GetEngine();

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
	if (result.get<sol::object>(0).get_type() == sol::type::table)
	{
		_table = result.get<sol::object>(0);
	}
	else
	{
		_table = scriptLoader.GetScriptEngine().CreateTable();
	}

	_fullyLoaded = true;
	engine.AddLoadingProgress(1);

	return _table;
}

const sol::table &ScriptModule::LazyLoad(IScriptLoader &scriptLoader)
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

	metatable["__index"] = [this, &scriptLoader](const sol::this_state &ts, const sol::object &, const sol::object &key)
	{
		return FullLoad(scriptLoader)[key];
	};
	metatable["__newindex"] = [](const sol::this_state &ts, const sol::object &, const sol::object &key)
	{
		// iScriptLoader.GetScriptEngine()->ThrowError("Attempt to modify readonly table");
		return sol::error("Attempt to modify readonly table");
	};

	return _table;
}

const sol::table &ScriptModule::FullLoad(IScriptLoader &scriptLoader)
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

	auto &&engine = GetEngine();

	engine.SetLoadingDescription(GetScriptProvider().GetScriptNameByID(_scriptID).value());

	auto previousScriptID = parent._loadingScript;
	parent._loadingScript = _scriptID;

	auto &&scriptEngine = parent.GetScriptEngine();

	_table = scriptEngine.CreateTable();
	auto &&metatable = scriptEngine.CreateTable(0, 2);
	_table[sol::metatable_key] = metatable;

	metatable["__index"] = [this, parent](const sol::this_state &ts, const sol::object &, const sol::object &key)
	{
		if (auto &&prevScriptID = FindPreviousInStack(parent._scriptLoopLoadStack, _scriptID); prevScriptID.has_value())
		{
			auto &&scriptProvider = GetScriptProvider();
			auto scriptName = scriptProvider.GetScriptNameByID(_scriptID);
			auto prevScriptName = scriptProvider.GetScriptNameByID(*prevScriptID);

			return sol::error(std::format("Cyclic dependency detected between <{}>\"{}\" and <{}>\"{}\"", _scriptID, *scriptName, *prevScriptID, *prevScriptName));
		}
		else
		{
			return sol::error("Attempt to access incomplete module");
		}
	};
	metatable["__newindex"] = [](const sol::this_state &ts, const sol::object &, const sol::object &key)
	{
		return sol::error("Attempt to modify readonly module");
	};

	parent._scriptLoopLoadStack.emplace_back(_scriptID);
	auto &&result = _func();
	assert(parent._scriptLoopLoadStack.back() == _scriptID);
	parent._scriptLoopLoadStack.pop_back();

	sol::table tbl;
	if (!result.valid()) [[unlikely]]
	{
		sol::error err = result;
		auto what = std::string_view(err.what());

		std::string_view scriptName = GetScriptProvider().GetScriptNameByID(_scriptID).value_or("");
		TE_ERROR("Error load script module <{}>\"{}\": {}", _scriptID, scriptName, what);

		GetScriptErrors().AddLoadtimeError(_scriptID, what);
	}
	else
	{
		if (result.get<sol::object>(1))
		{
			TE_WARN("'{}': Does not support receiving multiple return values", parent.GetLoadingScriptName().value());
		}

		auto &&value = result.get<sol::object>(0);
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
