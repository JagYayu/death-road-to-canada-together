#include "ScriptLoader.h"

#include "ModManager.h"
#include "ScriptEngine.h"
#include "ScriptProvider.h"

#include <optional>
#include <sol/error.hpp>
#include <sol/forward.hpp>
#include <sol/types.hpp>

#include <format>
#include <string_view>

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
		_table = scriptLoader.modManager.scriptEngine.CreateTable();
	}

	if (_isLoaded || _table[sol::metatable_key].valid())
	{
		return _table;
	}

	sol::table metatable = scriptLoader.modManager.scriptEngine.CreateTable();

	_table[sol::metatable_key] = metatable;

	metatable["__index"] = [&](sol::this_state ts, sol::object /*self*/, sol::object key)
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

	auto &&scriptEngine = scriptLoader.modManager.scriptEngine;

	sol::table tab;
	auto &&result = _func();
	if (!result.valid())
	{
		sol::error cc = result;
		scriptLoader._log.Error(std::string(cc.what()));
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

	_isLoaded = true;
	_isLoaded = false;

	return _table;
}

ScriptLoader::ScriptLoader(ModManager &modManager)
    : modManager(modManager),
      _log("ScriptLoader")
{
}

const std::string &ScriptLoader::GetLoadingScript() const
{
	return _loadingScript;
}

void ScriptLoader::LoadAll()
{
	_log.Debug("Loading provided scripts ...");

	if (!_loadedScripts.empty())
	{
		UnloadAll();
	}

	_loadedScripts.clear();
	_scriptErrors.clear();

	auto &&provider = modManager.scriptProvider;
	auto &&count = provider.GetCount();
	for (auto &&[name, data] : provider)
	{
		LoadImpl(name, data, true);
	}

	_log.Debug("Loaded provided scripts");
}

std::optional<std::reference_wrapper<ScriptLoader::Module>> ScriptLoader::Load(const std::string &scriptName)
{
	auto &&provider = modManager.scriptProvider;
	if (!provider.ContainsScript(scriptName))
	{
		return std::nullopt;
	}

	auto &&scriptCode = provider.GetScript(scriptName);
	return LoadImpl(scriptName, scriptCode, false);
}

std::optional<std::reference_wrapper<ScriptLoader::Module>> ScriptLoader::LoadImpl(const std::string &scriptName, const std::string_view &code, bool immediate)
{
	_log.Debug(std::format("Loading script \"{}\" ...", std::string_view(scriptName)));

	auto &&result = modManager.scriptEngine.LoadFunction(scriptName, code);
	if (!result.valid())
	{
		_log.Debug(std::format("Failed to load script \"{}\": {}", scriptName, result.get<sol::error>().what()));

		return std::nullopt;
	}

	auto &&func = result.get<sol::protected_function>();
	modManager.scriptEngine.InitScriptFunc(scriptName, func);
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

	_log.Debug(std::format("Loaded script \"{}\"", std::string_view(scriptName)));

	return _loadedScripts[scriptName];
}

void ScriptLoader::UnloadAll()
{
	auto &&scriptProvider = modManager.scriptProvider;
	for (auto it = _loadedScripts.begin(); it != _loadedScripts.end();)
	{
		if (scriptProvider.ContainsScript(it->first))
		{
			++it;
			continue;
		}

		_log.Trace(std::format("Unloading script \"{}\" ...", it->first));

		it = _loadedScripts.erase(it);
	}
}
