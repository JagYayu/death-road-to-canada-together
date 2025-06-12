#include "ScriptLoader.h"

#include "ModManager.h"
#include "ScriptEngine.h"
#include "ScriptProvider.h"
#include "util/Defs.h"

#include <cassert>
#include <sol/error.hpp>
#include <sol/forward.hpp>
#include <sol/types.hpp>

#include <optional>

using namespace tudov;

ScriptLoader::Module::Module()
    : _scriptID(),
      _func(),
      _fullyLoaded(),
      _table()
{
}

ScriptLoader::Module::Module(ScriptID scriptID, const sol::protected_function &func)
    : _scriptID(scriptID),
      _func(func),
      _fullyLoaded(),
      _table()
{
}

bool ScriptLoader::Module::IsLazyLoaded() const
{
	return _table.valid();
}

bool ScriptLoader::Module::IsFullyLoaded() const
{
	return _fullyLoaded;
}

const sol::table &ScriptLoader::Module::GetTable()
{
	return _table;
}

const sol::table &ScriptLoader::Module::RawLoad()
{
	if (_fullyLoaded)
	{
		return _table;
	}

	auto &&result = _func();
	_table = result.get<sol::object>(0);
	_fullyLoaded = true;
	return _table;
}

const sol::table &ScriptLoader::Module::LazyLoad(ScriptLoader &scriptLoader)
{
	if (!IsLazyLoaded())
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
		return FullLoad(scriptLoader)[key];
	};

	return _table;
}

const sol::table &ScriptLoader::Module::FullLoad(ScriptLoader &scriptLoader)
{
	if (_fullyLoaded)
	{
		return _table;
	}

	assert(_scriptID);
	auto &&previousLoadingScript = scriptLoader._loadingScript;
	scriptLoader._loadingScript = _scriptID;

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
		scriptLoader._log->Error("{}", cc.what());
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

	scriptLoader._loadingScript = previousLoadingScript;
	_fullyLoaded = true;

	return _table;
}

ScriptLoader::ScriptLoader(ScriptEngine &scriptEngine) noexcept
    : scriptEngine(scriptEngine),
      _log(Log::Get("ScriptLoader")),
      _loadingScript(),
      onPreLoadAllScripts(),
      _scriptReverseDependencies()
{
}

ScriptLoader::~ScriptLoader() noexcept
{
	// for (auto &&it = _scriptModules.begin(); it != _scriptModules.end(); ++it)
	// {
	// 	_log->Trace("Unloading script \"{}\" ...", it->first));
	// 	_log->Trace("Unloaded script \"{}\"", it->first));
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

	onPreLoadAllScripts();

	if (!_scriptModules.empty())
	{
		UnloadAll();
	}

	_scriptModules.clear();
	_scriptErrors.clear();

	auto &&count = scriptEngine.modManager.scriptProvider.GetCount();
	for (auto &&[scriptID, entry] : scriptEngine.modManager.scriptProvider)
	{
		auto &&scriptName = scriptEngine.modManager.scriptProvider.GetScriptNameByID(scriptID);
		LoadImpl(scriptID, scriptName.value(), entry.code);
	}

	ProcessFullLoads();

	onPreLoadAllScripts();

	_log->Debug("Loaded provided scripts");
}

SharedPtr<ScriptLoader::Module> ScriptLoader::Load(ScriptID scriptID)
{
	if (scriptID)
	{
		auto &&scriptCode = scriptEngine.modManager.scriptProvider.GetScriptCode(scriptID);
		auto &&scriptName = scriptEngine.modManager.scriptProvider.GetScriptNameByID(scriptID);
		return LoadImpl(scriptID, scriptName.value(), scriptCode);
	}

	_log->Warn("{}", "Attempt to load invalid script");
	return nullptr;
}

SharedPtr<ScriptLoader::Module> ScriptLoader::LoadImpl(ScriptID scriptID, StringView scriptName, StringView scriptCode)
{
	{
		auto &&it = _scriptModules.find(scriptID);
		if (it != _scriptModules.end())
		{
			return it->second;
		}
	}

	_log->Debug("Loading script \"{}\" ...", scriptName);

	_scriptErrors.erase(scriptID);

	auto &&result = scriptEngine.modManager.scriptEngine.LoadFunction(String(scriptName), scriptCode);
	if (!result.valid())
	{
		_log->Debug("Failed to load script \"{}\": {}", scriptID, result.get<sol::error>().what());

		return nullptr;
	}

	auto &&func = result.get<sol::protected_function>();
	auto &&module = MakeShared<Module>(scriptID, func);
	auto &&[it, inserted] = _scriptModules.try_emplace(scriptID, module);
	assert(inserted);

	if (ScriptProvider::IsStaticScript(scriptName))
	{
		module->RawLoad();
		_log->Trace("Raw loaded script \"{}\"", scriptName);
	}
	else
	{
		scriptEngine.modManager.scriptEngine.InitScriptFunc(scriptID, scriptName, func);
		_log->Trace("Lazy loaded script \"{}\"", scriptName);
	}

	return it->second;
}

void ScriptLoader::UnloadAll()
{
	for (auto &&it : _scriptModules)
	{
		if (!scriptEngine.modManager.scriptProvider.GetScriptNameByID(it.first))
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
	_log->Trace("Unloading script <{}>", scriptID);

	if (!_scriptModules.erase(scriptID))
	{
		_log->Trace("Unload script <{}> failed: script not found", scriptID);

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

	auto &&name = scriptEngine.modManager.scriptProvider.GetScriptNameByID(scriptID);
	_log->Trace("Unloaded script \"{}\"", name.value());
}

void ScriptLoader::HotReload(const Vector<ScriptID> &scriptIDs)
{
	_log->Debug("Hot reloading scripts ...");

	onPreHotReloadScripts(scriptIDs);

	// auto &&scriptCode = scriptEngine.modManager.scriptProvider.GetScriptCode(scriptID);
	// auto &&scriptName = scriptEngine.modManager.scriptProvider.GetScriptNameByID(scriptID);
	// auto &&scriptModule = LoadImpl(scriptID, scriptName.value(), scriptCode);

	for (auto &&additionalScriptID : scriptIDs)
	{
		Load(additionalScriptID);
	}
	ProcessFullLoads();

	onPostHotReloadScripts(scriptIDs);

	_log->Debug("Hot reloaded scripts ...");
}

void ScriptLoader::ProcessFullLoads()
{
	_log->Debug("Processing full loads ...");

	for (auto &&[scriptID, module] : _scriptModules)
	{
		if (!module->IsFullyLoaded())
		{
			module->FullLoad(*this);
			_log->Trace("Fully loaded script \"{}\"", scriptEngine.modManager.scriptProvider.GetScriptNameByID(scriptID).value());
		}
	}

	_log->Debug("Processed full loads");
}
