#include "ScriptLoader.h"

#include "ModManager.h"
#include "ScriptEngine.h"
#include "ScriptProvider.h"
#include "util/Defs.h"

#include <sol/error.hpp>
#include <sol/forward.hpp>
#include <sol/types.hpp>

#include <optional>

using namespace tudov;

ScriptLoader::Module::Module()
    : _func(),
      _isLoaded(false),
      _isLoading(false),
      _table()
{
}

ScriptLoader::Module::Module(const sol::protected_function &func)
    : _func(func),
      _isLoaded(false),
      _isLoading(false),
      _table()
{
}

bool ScriptLoader::Module::IsLoaded() const
{
	return _isLoaded;
}

bool ScriptLoader::Module::IsLoading() const
{
	return _isLoading;
}

const sol::table &ScriptLoader::Module::LazyLoad(ScriptLoader &scriptLoader)
{
	if (!_table.valid())
	{
		_table = scriptLoader.scriptEngine.modManager.scriptEngine.CreateTable();
	}

	if (_isLoaded || _table[sol::metatable_key].valid())
	{
		return _table;
	}

	sol::table metatable = scriptLoader.scriptEngine.modManager.scriptEngine.CreateTable();

	_table[sol::metatable_key] = metatable;

	metatable["__index"] = [&](const sol::this_state &ts, const sol::object &, const sol::object &key)
	{
		return ImmediateLoad(scriptLoader)[key];
	};

	return _table;
}

const sol::table &ScriptLoader::Module::ImmediateLoad(ScriptLoader &scriptLoader)
{
	if (_isLoaded)
	{
		return _table;
	}

	_isLoaded = true;

	auto &&scriptEngine = scriptLoader.scriptEngine.modManager.scriptEngine;

	sol::table tab;
	auto &&result = _func();
	if (!result.valid())
	{
		sol::error cc = result;
		scriptLoader._log->Error(String(cc.what()));
		tab = scriptEngine.CreateTable();
	}
	else
	{
		auto &&value = result.get<sol::object>(0);
		if (value.get_type() == sol::type::table)
		{
			tab = value.as<sol::table>();
		}
	}

	sol::table metatable = scriptEngine.CreateTable(0, 2);

	metatable["__index"] = tab;
	metatable["__newindex"] = []() {};

	_table = scriptEngine.CreateTable();
	_table[sol::metatable_key] = metatable;

	_isLoaded = false;

	return _table;
}

ScriptLoader::ScriptLoader(ScriptEngine &scriptEngine) noexcept
    : scriptEngine(scriptEngine),
      _log(Log::Get("ScriptLoader")),
      onPreLoadAllScripts()
{
}

ScriptLoader::~ScriptLoader() noexcept
{
	for (auto &&it = _loadedScripts.begin(); it != _loadedScripts.end(); ++it)
	{
		_log->Trace(Format("Unloading script \"{}\" ...", it->first));

		// TODO

		_log->Trace(Format("Unloaded script \"{}\"", it->first));
	}
}

Optional<String> ScriptLoader::GetLoadingScript() const
{
	return _loadingScript;
}

void ScriptLoader::LoadAll()
{
	_log->Debug("Loading provided scripts ...");

	if (!_loadedScripts.empty())
	{
		UnloadAll();
	}

	_loadedScripts.clear();
	_scriptErrors.clear();

	auto &&count = scriptEngine.modManager.scriptProvider.GetCount();
	for (auto &&[name, data] : scriptEngine.modManager.scriptProvider)
	{
		LoadImpl(name, data, true);
	}

	_log->Debug("Loaded provided scripts");
}

Optional<Reference<ScriptLoader::Module>> ScriptLoader::Load(const String &scriptName)
{
	auto &&scriptCode = scriptEngine.modManager.scriptProvider.TryRequireScript(scriptName);
	if (!scriptCode.has_value())
	{
		return null;
	}

	return LoadImpl(scriptName, scriptCode.value(), false);
}

Optional<Reference<ScriptLoader::Module>> ScriptLoader::LoadImpl(const String &scriptName, const StringView &code, bool immediate)
{
	_log->Debug(Format("Loading script \"{}\" ...", scriptName));

	_loadingScript = scriptName;

	auto &&result = scriptEngine.modManager.scriptEngine.LoadFunction(scriptName, code);
	if (!result.valid())
	{
		_loadingScript = null;

		_log->Debug(Format("Failed to load script \"{}\": {}", scriptName, result.get<sol::error>().what()));

		return null;
	}

	auto &&func = result.get<sol::protected_function>();
	scriptEngine.modManager.scriptEngine.InitScriptFunc(scriptName, func);
	Module module{func};

	auto &&it = _loadedScripts.find(scriptName);
	if (it == _loadedScripts.end())
	{
		_loadedScripts[scriptName] = module;
	}

	_scriptErrors.erase(scriptName);

	if (immediate)
	{
		module.ImmediateLoad(*this);
	}

	_loadingScript = null;

	_log->Debug(Format("Loaded script \"{}\"", StringView(scriptName)));

	return _loadedScripts[scriptName];
}

void ScriptLoader::UnloadAll()
{
	for (auto it = _loadedScripts.begin(); it != _loadedScripts.end();)
	{
		if (scriptEngine.modManager.scriptProvider.ContainsScript(it->first))
		{
			++it;
			continue;
		}

		_log->Trace(Format("Unloading script \"{}\" ...", it->first));

		it = _loadedScripts.erase(it);

		_log->Trace(Format("Unloaded script \"{}\"", it->first));
	}
}
