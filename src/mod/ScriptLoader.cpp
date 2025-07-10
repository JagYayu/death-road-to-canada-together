#include "ScriptLoader.hpp"

#include "ModManager.hpp"
#include "ScriptEngine.hpp"
#include "ScriptProvider.hpp"
#include "util/Defs.hpp"
#include "util/StringUtils.hpp"
#include "util/Utils.hpp"

#include <sol/error.hpp>
#include <sol/forward.hpp>
#include <sol/types.hpp>

#include <algorithm>
#include <cassert>
#include <format>
#include <optional>
#include <stdexcept>
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

const sol::table &ScriptLoader::Module::RawLoad(IScriptLoader &iScriptLoader)
{
	if (_fullyLoaded)
	{
		return _table;
	}

	auto &&scriptLoader = static_cast<ScriptLoader &>(iScriptLoader);

	assert(_scriptID);
	auto previousLoadingScript = scriptLoader._loadingScript;
	scriptLoader._loadingScript = _scriptID;
	auto &&result = _func();
	if (result.get<sol::object>(1))
	{
		scriptLoader._log->Warn("'{}': Does not support receiving multiple return values", iScriptLoader.GetLoadingScriptName().value());
	}

	scriptLoader._loadingScript = previousLoadingScript;
	if (result.get<sol::object>(0).get_type() == sol::type::table)
	{
		_table = result.get<sol::object>(0);
	}
	else
	{
		_table = iScriptLoader.GetScriptEngine()->CreateTable();
	}

	_fullyLoaded = true;
	return _table;
}

const sol::table &ScriptLoader::Module::LazyLoad(IScriptLoader &iScriptLoader)
{
	if (!_table.valid())
	{
		_table = iScriptLoader.GetScriptEngine()->CreateTable();
	}

	if (_table[sol::metatable_key].valid())
	{
		return _table;
	}

	sol::table metatable = iScriptLoader.GetScriptEngine()->CreateTable();

	_table[sol::metatable_key] = metatable;

	metatable["__index"] = [&](const sol::this_state &ts, const sol::object &, const sol::object &key)
	{
		return FullLoad(iScriptLoader)[key];
	};
	metatable["__newindex"] = [&](const sol::this_state &ts, const sol::object &, const sol::object &key)
	{
		iScriptLoader.GetScriptEngine()->ThrowError("Attempt to modify readonly table");
		return;
	};

	return _table;
}

const sol::table &ScriptLoader::Module::FullLoad(IScriptLoader &scriptLoader)
{
	if (_fullyLoaded)
	{
		return _table;
	}

	auto &&parent = static_cast<ScriptLoader &>(scriptLoader);

	assert(_scriptID);
	auto previousScriptID = parent._loadingScript;
	parent._loadingScript = _scriptID;

	auto &&scriptEngine = parent.GetScriptEngine();

	_table = scriptEngine->CreateTable();
	auto &&metatable = scriptEngine->CreateTable(0, 2);
	_table[sol::metatable_key] = metatable;

	metatable["__index"] = [&](const sol::this_state &ts, const sol::object &, const sol::object &key)
	{
		if (auto &&prevScriptID = FindPreviousInStack(parent._scriptLoopLoadStack, _scriptID); prevScriptID.has_value())
		{
			auto &&scriptProvider = parent.GetScriptProvider();
			auto scriptName = scriptProvider->GetScriptNameByID(_scriptID);
			auto prevScriptName = scriptProvider->GetScriptNameByID(*prevScriptID);
			parent.GetScriptEngine()->ThrowError(std::format("Cyclic dependency detected between <{}>\"{}\" and <{}>\"{}\"", _scriptID, *scriptName, *prevScriptID, *prevScriptName));
		}
		else
		{
			parent.GetScriptEngine()->ThrowError("Attempt to access incomplete module");
		}
	};
	metatable["__newindex"] = [&](const sol::this_state &ts, const sol::object &, const sol::object &key)
	{
		scriptLoader.GetScriptEngine()->ThrowError("Attempt to modify readonly module");
	};

	parent._scriptLoopLoadStack.emplace_back(_scriptID);
	auto &&result = _func();
	assert(parent._scriptLoopLoadStack.back() == _scriptID);
	parent._scriptLoopLoadStack.pop_back();

	sol::table tbl;
	if (!result.valid())
	{
		sol::error err = result;
		parent._log->Error("{}", err.what());
		parent._scriptErrors.try_emplace(_scriptID, std::make_tuple(parent._scriptErrors.size(), std::string(err.what())));
	}
	else
	{
		if (result.get<sol::object>(1))
		{
			parent._log->Warn("'{}': Does not support receiving multiple return values", parent.GetLoadingScriptName().value());
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
				parent._log->Warn("'{}': Does not support receiving non-table values", parent.GetLoadingScriptName().value());
			}
			tbl = scriptEngine->CreateTable();
		}
	}

	metatable["__index"] = tbl;

	parent._loadingScript = previousScriptID;
	_fullyLoaded = true;

	return _table;
}

ScriptLoader::ScriptLoader(Context &context) noexcept
    : _context(context),
      _log(Log::Get("ScriptLoader")),
      _loadingScript(),
      _onPreLoadAllScripts(),
      _onPostLoadAllScripts(),
      _onPreHotReloadScripts(),
      _onPostHotReloadScripts(),
      _onLoadedScript(),
      _onUnloadScript(),
      _scriptReverseDependencies()
{
}

ScriptLoader::~ScriptLoader() noexcept
{
	IScriptLoader::~IScriptLoader();
}

Context &ScriptLoader::GetContext() noexcept
{
	return _context;
}

DelegateEvent<> &ScriptLoader::GetOnPreLoadAllScripts() noexcept
{
	return _onPreLoadAllScripts;
}

DelegateEvent<> &ScriptLoader::GetOnPostLoadAllScripts() noexcept
{
	return _onPostLoadAllScripts;
}

DelegateEvent<const std::vector<ScriptID> &> &ScriptLoader::GetOnPreHotReloadScripts() noexcept
{
	return _onPreHotReloadScripts;
}

DelegateEvent<const std::vector<ScriptID> &> &ScriptLoader::GetOnPostHotReloadScripts() noexcept
{
	return _onPostHotReloadScripts;
}

DelegateEvent<ScriptID> &ScriptLoader::GetOnLoadedScript() noexcept
{
	return _onLoadedScript;
}

DelegateEvent<ScriptID> &ScriptLoader::GetOnUnloadScript() noexcept
{
	return _onUnloadScript;
}

bool ScriptLoader::Exists(ScriptID scriptID) noexcept
{
	return _scriptModules.contains(scriptID);
}

bool ScriptLoader::IsLazyLoaded(ScriptID scriptID) noexcept
{
	auto &&it = _scriptModules.find(scriptID);
	return it == _scriptModules.end() ? false : it->second->IsLazyLoaded();
}

bool ScriptLoader::IsFullyLoaded(ScriptID scriptID) noexcept
{
	auto &&it = _scriptModules.find(scriptID);
	return it == _scriptModules.end() ? false : it->second->IsFullyLoaded();
}

ScriptID ScriptLoader::GetLoadingScriptID() const noexcept
{
	return _loadingScript;
}

std::optional<std::string_view> ScriptLoader::GetLoadingScriptName() const noexcept
{
	return GetScriptProvider()->GetScriptNameByID(_loadingScript);
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
	auto &&scriptProvider = GetScriptProvider();
	if (!scriptProvider->IsValidScript(source) || !scriptProvider->IsValidScript(target)) [[unlikely]]
	{
		auto &&sourceName = scriptProvider->GetScriptNameByID(source);
		auto &&targetName = scriptProvider->GetScriptNameByID(target);
		_log->Warn("Attempt to link invalid scripts to reverse dependency map: <{}>\"{}\" <- <{}>\"{}\"",
		           source, sourceName.has_value() ? sourceName->data() : "#INVALID#",
		           target, targetName.has_value() ? targetName->data() : "#INVALID#");

		return;
	}
	if (scriptProvider->IsStaticScript(source) || scriptProvider->IsStaticScript(target)) [[unlikely]]
	{
		auto &&sourceName = scriptProvider->GetScriptNameByID(source);
		auto &&targetName = scriptProvider->GetScriptNameByID(target);
		_log->Warn("Attempt to link static scripts to reverse dependency map: <{}>\"{}\" <- <{}>\"{}\"",
		           source, sourceName.has_value() ? sourceName->data() : "#INVALID#",
		           target, targetName.has_value() ? targetName->data() : "#INVALID#");

		return;
	}

	auto &&dependencies = _scriptReverseDependencies[target];
	dependencies.insert(source);
	// _scriptDependencyGraph.AddLink(from, to);

	if (_log->CanTrace())
	{
		_log->Trace("Link scripts to reverse dependency map: <{}>\"{}\" <- <{}>\"{}\"",
		            source, scriptProvider->GetScriptNameByID(source)->data(),
		            target, scriptProvider->GetScriptNameByID(target)->data());
	}
}

void ScriptLoader::LoadAll()
{
	_log->Debug("Loading provided scripts ...");

	_onPreLoadAllScripts();

	if (!_scriptModules.empty())
	{
		UnloadAll();
	}

	_scriptModules.clear();
	_scriptErrors.clear();
	// _scriptErrorsCascaded.clear();
	// _scriptDependencyGraph.Clear();

	auto &&scriptProvider = GetScriptProvider();

	auto &&count = scriptProvider->GetCount();
	for (auto &&[scriptID, entry] : *scriptProvider)
	{
		LoadImpl(scriptID, entry.name, entry.code, entry.namespace_);
	}

	ProcessFullLoads();

	_onPostLoadAllScripts();

	_log->Debug("Loaded provided scripts");

	Log::CleanupExpired();
}

std::shared_ptr<IScriptLoader::IModule> ScriptLoader::Load(std::string_view scriptName)
{
	auto &&scriptID = GetScriptProvider()->GetScriptIDByName(scriptName);
	if (!scriptID)
	{
		return nullptr;
	}
	return Load(scriptID);
}

std::shared_ptr<IScriptLoader::IModule> ScriptLoader::Load(ScriptID scriptID)
{
	if (!scriptID)
	{
		_log->Warn("Attempt to load invalid script <{}>", scriptID);
		return nullptr;
	}
	auto &&scriptProvider = GetScriptProvider();
	auto &&scriptName = scriptProvider->GetScriptNameByID(scriptID);
	if (!scriptName.has_value())
	{
		_log->Warn("Attempt to load invalid script <{}>: missing script name", scriptID);
		return nullptr;
	}

	auto &&scriptCode = scriptProvider->GetScriptCode(scriptID);
	auto &&scriptNamespace = scriptProvider->GetScriptNamespace(scriptID);
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

	auto &&result = GetScriptEngine()->LoadFunction(std::string(scriptName), scriptCode);
	if (!result.valid())
	{
		_log->Debug("Failed to load script <{}>\"{}\": {}", scriptID, scriptName, result.get<sol::error>().what());

		return nullptr;
	}

	auto &&function = result.get<sol::protected_function>();
	auto &&module = std::make_shared<Module>(scriptID, function);
	auto &&[it, inserted] = _scriptModules.try_emplace(scriptID, module);
	assert(inserted);

	if (GetScriptProvider()->IsStaticScript(scriptName))
	{
		module->RawLoad(*this);
		_log->Trace("Raw loaded script <{}>\"{}\"", scriptID, scriptName);
	}
	else
	{
		std::string_view sandboxKey = emptyString;
		if (namespace_ != emptyString)
		{
			auto &&mod = GetModManager()->GetLoadedMod(namespace_);
			if (!mod.expired() && mod.lock()->GetConfig().scripts.sandbox)
			{
				sandboxKey = namespace_;
			}
		}
		GetScriptEngine()->InitializeScriptFunction(scriptID, scriptName, function, sandboxKey);
		_log->Trace("Lazy loaded script <{}>\"{}\"", scriptID, scriptName);
	}

	_onLoadedScript(scriptID);

	return it->second;
}

void ScriptLoader::UnloadAll()
{
	auto &&scriptProvider = GetScriptProvider();

	for (auto &&it : _scriptModules)
	{
		if (!scriptProvider->GetScriptNameByID(it.first))
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

std::vector<ScriptID> ScriptLoader::UnloadBy(std::string_view namespace_)
{
	std::vector<ScriptID> unloadedScripts{};

	std::vector<ScriptID> unloadingScripts{};
	auto &&scriptProvider = GetScriptProvider();
	for (auto &&[scriptID, _] : _scriptModules)
	{
		if (scriptProvider->GetScriptNamespace(scriptID) == namespace_)
		{
			unloadingScripts.emplace_back(scriptID);
		}
	}
	for (auto scriptID : unloadingScripts)
	{
		UnloadImpl(scriptID, unloadedScripts);
	}

	return std::move(unloadedScripts);
}

std::vector<ScriptID> ScriptLoader::UnloadInvalids()
{
	// TODO
	throw std::runtime_error("NOT IMPLEMENT YET");
	// std::vector<ScriptID> unloadedScripts{};
	// return std::move(unloadedScripts);
}

void ScriptLoader::UnloadImpl(ScriptID scriptID, std::vector<ScriptID> &unloadedScripts)
{
	auto &&scriptName = GetScriptProvider()->GetScriptNameByID(scriptID);
	if (!scriptName.has_value()) [[unlikely]]
	{
		_log->Warn("Attempt to unload invalid script <{}> ...", scriptID);

		return;
	}

	_log->Trace("Unloading script <{}>\"{}\" ...", scriptID, scriptName->data());

	if (!_scriptModules.erase(scriptID))
	{
		_log->Trace("Already unloaded");
		return;
	}

	_onUnloadScript(scriptID);

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

	_log->Trace("Unloaded");
}

void ScriptLoader::HotReload(const std::vector<ScriptID> &scriptIDs)
{
	_log->Debug("Hot reloading scripts ...");

	if (_log->CanTrace())
	{
		auto &&scriptProvider = GetScriptProvider();
		for (auto scriptID : scriptIDs)
		{
			auto &&scriptName = scriptProvider->GetScriptNameByID(scriptID);
			_log->Trace("<{}>\"{}\"", scriptID, scriptName.has_value() ? scriptName->data() : "#INVALID#");
		}
	}

	_onPreHotReloadScripts(scriptIDs);

	for (auto &&additionalScriptID : scriptIDs)
	{
		Load(additionalScriptID);
	}
	ProcessFullLoads();

	_onPostHotReloadScripts(scriptIDs);

	_log->Debug("Hot reloaded scripts");

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
			_log->Trace("Fully loaded script \"{}\"", GetScriptProvider()->GetScriptNameByID(scriptID).value());
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
