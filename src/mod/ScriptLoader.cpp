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

using ScriptID = ScriptLoader::ScriptID;

ScriptLoader::Module::Module()
    : _func(),
      _table()
{
}

ScriptLoader::Module::Module(const sol::protected_function &func)
    : _func(func),
      _table()
{
}

bool ScriptLoader::Module::IsLoaded() const
{
	return _table.valid();
}

const sol::table &ScriptLoader::Module::GetTable()
{
	return _table;
}

const sol::table &ScriptLoader::Module::RawLoad()
{
	if (_table.valid())
	{
		return _table;
	}

	auto &&result = _func();
	_table = result.get<sol::object>(0);
	return _table;
}

const sol::table &ScriptLoader::Module::LazyLoad(ScriptLoader &scriptLoader)
{
	if (!_table.valid())
	{
		_table = scriptLoader.scriptEngine.modManager.scriptEngine.CreateTable();
	}

	if (_table[sol::metatable_key].valid())
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
	if (_table.valid())
	{
		return _table;
	}

	auto &&scriptEngine = scriptLoader.scriptEngine.modManager.scriptEngine;

	_table = scriptEngine.CreateTable();
	sol::table metatable = scriptEngine.CreateTable(0, 2);

	_table[sol::metatable_key] = metatable;

	metatable["__newindex"] = []() {};

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

	metatable["__index"] = tab;

	return _table;
}

ScriptLoader::ScriptLoader(ScriptEngine &scriptEngine) noexcept
    : scriptEngine(scriptEngine),
      _log(Log::Get("ScriptLoader")),
      onPreLoadAllScripts(),
      _scriptReverseDependencies()
{
}

ScriptLoader::~ScriptLoader() noexcept
{
	// for (auto &&it = _scriptModules.begin(); it != _scriptModules.end(); ++it)
	// {
	// 	_log->Trace(Format("Unloading script \"{}\" ...", it->first));
	// 	_log->Trace(Format("Unloaded script \"{}\"", it->first));
	// }
}

ScriptID ScriptLoader::GetLoadingScript() const noexcept
{
	return _loadingScript;
}

void GetScriptDependencies(const UnorderedMap<ScriptID, UnorderedSet<ScriptID>> &scriptDependencies, ScriptID scriptID, Vector<ScriptID> &sources, UnorderedSet<ScriptID> &visited)
{
	if (visited.contains(scriptID))
	{
		return;
	}
	visited.emplace(scriptID);

	auto &&it = scriptDependencies.find(scriptID);
	if (it != scriptDependencies.end())
	{
		for (auto &&source : it->second)
		{
			sources.emplace_back(source);
			GetScriptDependencies(scriptDependencies, source, sources, visited);
		}
	}
}

Vector<ScriptID> ScriptLoader::GetDependencies(ScriptID scriptID) const
{
	auto &&sources = Vector<ScriptID>();
	auto &&visited = UnorderedSet<ScriptID>();
	GetScriptDependencies(_scriptReverseDependencies, scriptID, sources, visited);
	return Move(sources);
}

void ScriptLoader::AddReverseDependency(ScriptID source, ScriptID target)
{
	if (!scriptEngine.modManager.scriptProvider.IsValidScriptID(source) || !scriptEngine.modManager.scriptProvider.IsValidScriptID(target))
	{
		_log->Warn("Attempt to add invalid scriptID to reverse dependency map");
		return;
	}
	auto &&dependencies = _scriptReverseDependencies[target];
	dependencies.insert(source);
}

void ScriptLoader::LoadAll()
{
	_log->Debug("Loading provided scripts ...");

	if (!_scriptModules.empty())
	{
		UnloadAll();
	}

	_scriptModules.clear();
	_scriptErrors.clear();

	auto &&count = scriptEngine.modManager.scriptProvider.GetCount();
	for (auto &&[scriptID, entry] : scriptEngine.modManager.scriptProvider)
	{
		auto &&scriptName = scriptEngine.modManager.scriptProvider.GetScriptName(scriptID);
		LoadImpl(scriptID, scriptName.value(), entry.code);
	}

	_log->Debug("Loaded provided scripts");
}

SharedPtr<ScriptLoader::Module> ScriptLoader::Load(ScriptID scriptID)
{
	if (!scriptID)
	{
		return nullptr;
	}

	auto &&scriptCode = scriptEngine.modManager.scriptProvider.GetScriptCode(scriptID);
	auto &&scriptName = scriptEngine.modManager.scriptProvider.GetScriptName(scriptID);
	return LoadImpl(scriptID, scriptName.value(), scriptCode);
}

SharedPtr<ScriptLoader::Module> ScriptLoader::LoadImpl(ScriptID scriptID, StringView scriptName, StringView scriptCode)
{
	auto &&it = _scriptModules.find(scriptID);
	if (it != _scriptModules.end())
	{
		return _scriptModules[scriptID];
	}

	_loadingScript = scriptID;

	_log->Debug(Format("Loading script \"{}\" ...", scriptName));

	_scriptErrors.erase(scriptID);

	auto &&result = scriptEngine.modManager.scriptEngine.LoadFunction(String(scriptName), scriptCode);
	if (!result.valid())
	{
		_loadingScript = ScriptProvider::invalidScriptID;

		_log->Debug(Format("Failed to load script \"{}\": {}", scriptID, result.get<sol::error>().what()));

		return nullptr;
	}

	auto &&func = result.get<sol::protected_function>();
	auto &&module = MakeShared<Module>(func);

	_scriptModules[scriptID] = module;

	if (ScriptProvider::IsStaticScript(scriptName))
	{
		module->RawLoad();
		_log->Trace(Format("Raw loaded script \"{}\"", scriptName));
	}
	else
	{
		scriptEngine.modManager.scriptEngine.InitScriptFunc(scriptID, scriptName, func);
		_log->Trace(Format("Lazy loaded script \"{}\"", scriptName));
	}

	ProcessImmediateLoads();

	_loadingScript = ScriptProvider::invalidScriptID;

	return _scriptModules[scriptID];
}

void ScriptLoader::UnloadAll()
{
	for (auto &&it : _scriptModules)
	{
		if (!scriptEngine.modManager.scriptProvider.GetScriptName(it.first))
		{
			Unload(it.first);
		}
	}
}

Vector<ScriptID> ScriptLoader::Unload(ScriptID scriptID)
{
	Vector<ScriptID> unloadedScripts{};
	UnloadImpl(scriptID, unloadedScripts);
	return Move(unloadedScripts);
}

void ScriptLoader::UnloadImpl(ScriptID scriptID, Vector<ScriptID> &unloadedScripts)
{
	if (!_scriptModules.erase(scriptID))
	{
		return;
	}

	for (auto &&dependency : GetDependencies(scriptID))
	{
		Unload(dependency);
		unloadedScripts.emplace_back(dependency);
	}

	_scriptReverseDependencies.erase(scriptID);
	for (auto &&[_, dependencies] : _scriptReverseDependencies)
	{
		dependencies.erase(scriptID);
	}

	_scriptErrors.erase(scriptID);
	_scriptErrorsCascaded.erase(scriptID);

	auto &&name = scriptEngine.modManager.scriptProvider.GetScriptName(scriptID);
	_log->Trace(Format("Unloaded script \"{}\"", name.value()));
}

void ScriptLoader::ProcessImmediateLoads()
{
	_log->Debug("Processing immediate loads ...");

	for (auto &&[scriptID, module] : _scriptModules)
	{
		if (!module->IsLoaded())
		{
			module->ImmediateLoad(*this);
			_log->Trace(Format("Immediate loaded script \"{}\"", scriptEngine.modManager.scriptProvider.GetScriptName(scriptID).value()));
		}
	}

	_log->Debug("Processed immediate loads");
}
