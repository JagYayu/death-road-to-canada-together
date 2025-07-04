#include "ScriptLoader.h"

#include "ModManager.h"
#include "ScriptEngine.h"
#include "ScriptProvider.h"
#include "util/Defs.h"
#include "util/StringUtils.hpp"
#include "util/Utils.hpp"

#include <sol/error.hpp>
#include <sol/forward.hpp>
#include <sol/types.hpp>

#include <algorithm>
#include <cassert>
#include <format>
#include <optional>
#include <tuple>
#include <vector>

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

const sol::table &ScriptLoader::Module::RawLoad(ScriptLoader &scriptLoader)
{
	if (_fullyLoaded)
	{
		return _table;
	}

	assert(_scriptID);
	auto previousLoadingScript = scriptLoader._loadingScript;
	scriptLoader._loadingScript = _scriptID;
	auto &&result = _func();
	if (result.get<sol::object>(1))
	{
		scriptLoader._log->Warn("'{}': Does not support receiving multiple return values", scriptLoader.GetLoadingScriptName().value());
	}

	scriptLoader._loadingScript = previousLoadingScript;
	if (result.get<sol::object>(0).get_type() == sol::type::table)
	{
		_table = result.get<sol::object>(0);
	}
	else
	{
		_table = scriptLoader.scriptEngine.CreateTable();
	}

	_fullyLoaded = true;
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
		return FullLoad(scriptLoader)[key];
	};
	metatable["__newindex"] = [&](const sol::this_state &ts, const sol::object &, const sol::object &key)
	{
		scriptLoader.scriptEngine.ThrowError("Attempt to modify readonly table");
		return;
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
	auto previousScriptID = scriptLoader._loadingScript;
	scriptLoader._loadingScript = _scriptID;

	auto &&scriptEngine = scriptLoader.scriptEngine.modManager.scriptEngine;

	_table = scriptEngine.CreateTable();
	auto &&metatable = scriptEngine.CreateTable(0, 2);
	_table[sol::metatable_key] = metatable;

	metatable["__index"] = [&](const sol::this_state &ts, const sol::object &, const sol::object &key)
	{
		if (auto &&prevScriptID = FindPreviousInStack(scriptLoader._scriptLoopLoadStack, _scriptID); prevScriptID.has_value())
		{
			auto &scriptProvider = scriptLoader.scriptEngine.modManager.scriptProvider;
			auto scriptName = scriptProvider.GetScriptNameByID(_scriptID);
			auto prevScriptName = scriptProvider.GetScriptNameByID(*prevScriptID);
			scriptLoader.scriptEngine.ThrowError(std::format("Cyclic dependency detected between <{}>\"{}\" and <{}>\"{}\"", _scriptID, *scriptName, *prevScriptID, *prevScriptName));
		}
		else
		{
			scriptLoader.scriptEngine.ThrowError("Attempt to access incomplete module");
		}
	};
	metatable["__newindex"] = [&](const sol::this_state &ts, const sol::object &, const sol::object &key)
	{
		scriptLoader.scriptEngine.ThrowError("Attempt to modify readonly module");
	};

	scriptLoader._scriptLoopLoadStack.emplace_back(_scriptID);
	auto &&result = _func();
	assert(scriptLoader._scriptLoopLoadStack.back() == _scriptID);
	scriptLoader._scriptLoopLoadStack.pop_back();

	sol::table tbl;
	if (!result.valid())
	{
		sol::error err = result;
		scriptLoader._log->Error("{}", err.what());
		scriptLoader._scriptErrors.try_emplace(_scriptID, std::make_tuple(scriptLoader._scriptErrors.size(), std::string(err.what())));
	}
	else
	{
		if (result.get<sol::object>(1))
		{
			scriptLoader._log->Warn("'{}': Does not support receiving multiple return values", scriptLoader.GetLoadingScriptName().value());
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
				scriptLoader._log->Warn("'{}': Does not support receiving non-table values", scriptLoader.GetLoadingScriptName().value());
			}
			tbl = scriptEngine.CreateTable();
		}
	}

	metatable["__index"] = tbl;

	scriptLoader._loadingScript = previousScriptID;
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
}

ScriptID ScriptLoader::GetLoadingScriptID() const noexcept
{
	return _loadingScript;
}

std::optional<std::string_view> ScriptLoader::GetLoadingScriptName() const noexcept
{
	return scriptEngine.modManager.scriptProvider.GetScriptNameByID(_loadingScript);
}

void GetScriptDependencies(const std::unordered_map<ScriptID, std::unordered_set<ScriptID>> &scriptDependencies, ScriptID scriptID, std::vector<ScriptID> &sources, std::unordered_set<ScriptID> &visited)
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

std::vector<ScriptID> ScriptLoader::GetDependencies(ScriptID scriptID) const
{
	auto &&sources = std::vector<ScriptID>();
	auto &&visited = std::unordered_set<ScriptID>();
	GetScriptDependencies(_scriptReverseDependencies, scriptID, sources, visited);
	return std::move(sources);
	// auto &&links = _scriptDependencyGraph.GetBackwardTraversal(scriptID);
	// return links.has_value() ? links.value() : std::vector<ScriptID>();
}

void ScriptLoader::AddReverseDependency(ScriptID source, ScriptID target)
{
	if (!scriptEngine.modManager.scriptProvider.IsValidScriptID(source) || !scriptEngine.modManager.scriptProvider.IsValidScriptID(target))
	{
		_log->Warn("Attempt to link invalid scripts to reverse dependency map: <{}> -> <{}>", source, target);
		return;
	}
	auto &&dependencies = _scriptReverseDependencies[target];
	dependencies.insert(source);
	// _scriptDependencyGraph.AddLink(from, to);
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
	// _scriptErrorsCascaded.clear();
	// _scriptDependencyGraph.Clear();

	auto &&scriptProvider = scriptEngine.modManager.scriptProvider;

	auto &&count = scriptProvider.GetCount();
	for (auto &&[scriptID, entry] : scriptProvider)
	{
		LoadImpl(scriptID, entry.name, entry.code, entry.namespace_);
	}

	ProcessFullLoads();

	onPostLoadAllScripts();

	_log->Debug("Loaded provided scripts");

	Log::CleanupExpired();
}

std::shared_ptr<ScriptLoader::Module> ScriptLoader::Load(std::string_view scriptName)
{
	auto &&scriptID = scriptEngine.modManager.scriptProvider.GetScriptIDByName(scriptName);
	if (!scriptID)
	{
		return nullptr;
	}
	return Load(scriptID);
}

std::shared_ptr<ScriptLoader::Module> ScriptLoader::Load(ScriptID scriptID)
{
	if (!scriptID)
	{
		_log->Warn("Attempt to load invalid script <{}>", scriptID);
		return nullptr;
	}
	auto &&scriptName = scriptEngine.modManager.scriptProvider.GetScriptNameByID(scriptID);
	if (!scriptName.has_value())
	{
		_log->Warn("Attempt to load invalid script <{}>: missing script name", scriptID);
		return nullptr;
	}

	auto &&scriptCode = scriptEngine.modManager.scriptProvider.GetScriptCode(scriptID);
	auto &&scriptNamespace = scriptEngine.modManager.scriptProvider.GetScriptNamespace(scriptID);
	return LoadImpl(scriptID, scriptName.value(), scriptCode, scriptNamespace);
}

std::shared_ptr<ScriptLoader::Module> ScriptLoader::LoadImpl(ScriptID scriptID, std::string_view scriptName, std::string_view scriptCode, std::string_view namespace_)
{
	{
		auto &&it = _scriptModules.find(scriptID);
		if (it != _scriptModules.end())
		{
			return it->second;
		}
	}

	_log->Debug("Loading script <{}>\"{}\" ...", scriptID, scriptName);

	_scriptErrors.erase(scriptID);
	// _scriptErrorsCascaded.erase(scriptID);

	auto &&result = scriptEngine.LoadFunction(std::string(scriptName), scriptCode);
	if (!result.valid())
	{
		_log->Debug("Failed to load script <{}>\"{}\": {}", scriptID, scriptName, result.get<sol::error>().what());

		return nullptr;
	}

	auto &&function = result.get<sol::protected_function>();
	auto &&module = std::make_shared<Module>(scriptID, function);
	auto &&[it, inserted] = _scriptModules.try_emplace(scriptID, module);
	assert(inserted);

	if (ScriptProvider::IsStaticScript(scriptName))
	{
		module->RawLoad(*this);
		_log->Trace("Raw loaded script <{}>\"{}\"", scriptID, scriptName);
	}
	else
	{
		std::string_view sandboxKey = emptyString;
		if (namespace_ != emptyString)
		{
			auto &&mod = scriptEngine.modManager.GetLoadedMod(namespace_);
			if (!mod.expired() && mod.lock()->GetConfig().scripts.sandbox)
			{
				sandboxKey = namespace_;
			}
		}
		scriptEngine.InitializeScriptFunction(scriptID, scriptName, function, sandboxKey);
		_log->Trace("Lazy loaded script <{}>\"{}\"", scriptID, scriptName);
	}

	onLoadedScript(scriptID);

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

std::vector<ScriptID> ScriptLoader::Unload(ScriptID scriptID)
{
	std::vector<ScriptID> unloadedScripts{};
	UnloadImpl(scriptID, unloadedScripts);
	return std::move(unloadedScripts);
}

void ScriptLoader::UnloadImpl(ScriptID scriptID, std::vector<ScriptID> &unloadedScripts)
{
	_log->Trace("Unloading script <{}> ...", scriptID);

	if (!_scriptModules.erase(scriptID))
	{
		_log->Error("Unload script <{}> failed: script not found", scriptID);

		return;
	}

	onUnloadScript(scriptID);

	for (auto &&dependency : GetDependencies(scriptID))
	{
		Unload(dependency);
		unloadedScripts.emplace_back(dependency);
	}

	_scriptErrors.erase(scriptID);
	// _scriptErrorsCascaded.erase(scriptID);
	// _scriptDependencyGraph.UnlinkOutgoing(scriptID);
	_scriptReverseDependencies.erase(scriptID);
	for (auto &&[_, dependencies] : _scriptReverseDependencies)
	{
		dependencies.erase(scriptID);
	}

	auto &&name = scriptEngine.modManager.scriptProvider.GetScriptNameByID(scriptID);
	_log->Trace("Unloaded script <{}> \"{}\"", scriptID, name.value());
}

void ScriptLoader::HotReload(const std::vector<ScriptID> &scriptIDs)
{
	_log->Debug("Hot reloading scripts ...");

	onPreHotReloadScripts(scriptIDs);

	for (auto &&additionalScriptID : scriptIDs)
	{
		Load(additionalScriptID);
	}
	ProcessFullLoads();

	onPostHotReloadScripts(scriptIDs);

	_log->Debug("Hot reloaded scripts ...");

	Log::CleanupExpired();
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

bool ScriptLoader::HasAnyLoadError() const noexcept
{
	return !_scriptErrors.empty();
}

std::vector<std::string> ScriptLoader::GetLoadErrors() noexcept
{
	if (!_scriptErrorsCache.has_value())
	{
		std::vector<std::pair<std::size_t, std::string>> temp;

		for (const auto &[scriptID, entry] : _scriptErrors)
		{
			temp.emplace_back(std::get<0>(entry), std::get<1>(entry));
		}

		std::ranges::sort(temp, [](const auto &a, const auto &b)
		{
			return a.first < b.first;
		});

		std::vector<std::string> result;
		result.reserve(temp.size());
		for (const auto &[order, message] : temp)
		{
			result.emplace_back(message);
		}

		_scriptErrorsCache = std::make_optional(result);
	}

	return _scriptErrorsCache.value();
}
