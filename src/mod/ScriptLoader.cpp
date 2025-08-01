#include "mod/ScriptLoader.hpp"
#include "mod/ModManager.hpp"
#include "mod/ScriptEngine.hpp"
#include "mod/ScriptProvider.hpp"
#include "program/Engine.hpp"
#include "resource/Text.hpp"
#include "util/Definitions.hpp"
#include "util/StringUtils.hpp"
#include "util/Utils.hpp"

#include <corecrt_terminate.h>
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

const sol::table &ScriptLoader::Module::RawLoad(IScriptLoader &scriptLoader)
{
	if (_fullyLoaded)
	{
		return _table;
	}

	assert(_scriptID);

	auto &&parent = static_cast<ScriptLoader &>(scriptLoader);
	auto &&engine = scriptLoader.GetEngine();

	engine.SetLoadingDescription(scriptLoader.GetScriptProvider().GetScriptNameByID(_scriptID).value());

	auto previousLoadingScript = parent._loadingScript;
	parent._loadingScript = _scriptID;
	auto &&result = _func();
	if (!result.valid()) [[unlikely]]
	{
		sol::error err = result;
		parent._log->Fatal("'{}': {}", scriptLoader.GetLoadingScriptName().value(), err.what());
		abort();
	}

	if (result.get<sol::object>(1))
	{
		parent._log->Warn("'{}': Does not support receiving multiple return values", scriptLoader.GetLoadingScriptName().value());
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

const sol::table &ScriptLoader::Module::LazyLoad(IScriptLoader &iScriptLoader)
{
	if (!_table.valid())
	{
		_table = iScriptLoader.GetScriptEngine().CreateTable();
	}

	if (_table[sol::metatable_key].valid())
	{
		return _table;
	}

	sol::table metatable = iScriptLoader.GetScriptEngine().CreateTable();

	_table[sol::metatable_key] = metatable;

	metatable["__index"] = [this, &iScriptLoader](const sol::this_state &ts, const sol::object &, const sol::object &key)
	{
		return FullLoad(iScriptLoader)[key];
	};
	metatable["__newindex"] = [](const sol::this_state &ts, const sol::object &, const sol::object &key)
	{
		// iScriptLoader.GetScriptEngine()->ThrowError("Attempt to modify readonly table");
		return sol::error("Attempt to modify readonly table");
	};

	return _table;
}

const sol::table &ScriptLoader::Module::FullLoad(IScriptLoader &scriptLoader)
{
	if (_fullyLoaded)
	{
		return _table;
	}

	assert(_scriptID);

	auto &&parent = static_cast<ScriptLoader &>(scriptLoader);
	auto &&engine = parent.GetEngine();

	engine.SetLoadingDescription(parent.GetScriptProvider().GetScriptNameByID(_scriptID).value());

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
			auto &&scriptProvider = parent.GetScriptProvider();
			auto scriptName = scriptProvider.GetScriptNameByID(_scriptID);
			auto prevScriptName = scriptProvider.GetScriptNameByID(*prevScriptID);
			// parent.GetScriptEngine()->ThrowError(std::format("Cyclic dependency detected between <{}>\"{}\" and <{}>\"{}\"", _scriptID, *scriptName, *prevScriptID, *prevScriptName));
			return sol::error(std::format("Cyclic dependency detected between <{}>\"{}\" and <{}>\"{}\"", _scriptID, *scriptName, *prevScriptID, *prevScriptName));
		}
		else
		{
			return sol::error("Attempt to access incomplete module");
			// parent.GetScriptEngine()->ThrowError("Attempt to access incomplete module");
		}
	};
	metatable["__newindex"] = [](const sol::this_state &ts, const sol::object &, const sol::object &key)
	{
		// scriptLoader.GetScriptEngine()->ThrowError("Attempt to modify readonly module");
		return sol::error("Attempt to modify readonly module");
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
			tbl = scriptEngine.CreateTable();
		}
	}

	metatable["__index"] = tbl;

	parent._loadingScript = previousScriptID;
	_fullyLoaded = true;
	engine.AddLoadingProgress(1);

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

bool ScriptLoader::IsScriptExists(ScriptID scriptID) noexcept
{
	return _scriptModules.contains(scriptID);
}

bool ScriptLoader::IsScriptLazyLoaded(ScriptID scriptID) noexcept
{
	auto &&it = _scriptModules.find(scriptID);
	return it == _scriptModules.end() ? false : it->second->IsLazyLoaded();
}

bool ScriptLoader::IsScriptFullyLoaded(ScriptID scriptID) noexcept
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
	return GetScriptProvider().GetScriptNameByID(_loadingScript);
}

void GetScriptDependenciesImpl(const std::unordered_map<ScriptID, std::unordered_set<ScriptID>> &scriptDependencies, ScriptID scriptID, std::vector<ScriptID> &sources, std::unordered_set<ScriptID> &visited)
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
			GetScriptDependenciesImpl(scriptDependencies, source, sources, visited);
		}
	}
}

std::vector<ScriptID> ScriptLoader::GetScriptDependencies(ScriptID scriptID) const
{
	auto &&sources = std::vector<ScriptID>();
	auto &&visited = std::unordered_set<ScriptID>();
	GetScriptDependenciesImpl(_scriptReverseDependencies, scriptID, sources, visited);
	return std::move(sources);
	// auto &&links = _scriptDependencyGraph.GetBackwardTraversal(scriptID);
	// return links.has_value() ? links.value() : std::vector<ScriptID>();
}

void ScriptLoader::AddReverseDependency(ScriptID source, ScriptID target)
{
	if (source == target)
	{
		return;
	}

	auto &&scriptProvider = GetScriptProvider();
	if (!scriptProvider.IsValidScript(source) || !scriptProvider.IsValidScript(target)) [[unlikely]]
	{
		auto &&sourceName = scriptProvider.GetScriptNameByID(source);
		auto &&targetName = scriptProvider.GetScriptNameByID(target);
		_log->Warn("Attempt to link invalid scripts to reverse dependency map: <{}>\"{}\" <- <{}>\"{}\"",
		           source, sourceName.has_value() ? sourceName->data() : "#INVALID#",
		           target, targetName.has_value() ? targetName->data() : "#INVALID#");

		return;
	}
	if (scriptProvider.IsStaticScript(source) || scriptProvider.IsStaticScript(target)) [[unlikely]]
	{
		auto &&sourceName = scriptProvider.GetScriptNameByID(source);
		auto &&targetName = scriptProvider.GetScriptNameByID(target);
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
		            source, scriptProvider.GetScriptNameByID(source)->data(),
		            target, scriptProvider.GetScriptNameByID(target)->data());
	}
}

void ScriptLoader::LoadAllScripts()
{
	_log->Debug("Loading provided scripts ...");

	auto &&scriptProvider = GetScriptProvider();

	GetEngine().SetLoadingInfo(Engine::LoadingInfoArgs{
	    .title = "Loading scripts",
	    .description = "",
	    .progressValue = 0.0f,
	    .progressTotal = std::float_t(scriptProvider.GetCount()),
	});

	_onPreLoadAllScripts();

	if (!_scriptModules.empty())
	{
		UnloadAllScripts();
	}

	_scriptModules.clear();
	_scriptErrors.clear();

	auto &&count = scriptProvider.GetCount();
	for (auto &&[scriptID, entry] : scriptProvider)
	{
		LoadImpl(scriptID, entry.name, entry.code->View(), entry.modUID);
	}

	ProcessFullLoads();

	_onPostLoadAllScripts();

	_log->Debug("Loaded provided scripts");

	Log::CleanupExpired();
}

std::shared_ptr<IScriptLoader::IModule> ScriptLoader::Load(std::string_view scriptName)
{
	auto &&scriptID = GetScriptProvider().GetScriptIDByName(scriptName);
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
		_log->Warn("Attempt to load invalid script <{}>: id is 0", scriptID);
		return nullptr;
	}
	auto &&scriptProvider = GetScriptProvider();
	auto &&scriptName = scriptProvider.GetScriptNameByID(scriptID);
	if (!scriptName.has_value())
	{
		_log->Warn("Attempt to load invalid script <{}>: not provided", scriptID);
		return nullptr;
	}

	auto &&scriptCode = scriptProvider.GetScriptCode(scriptID);
	auto &&modUID = scriptProvider.GetScriptModUID(scriptID);
	return LoadImpl(scriptID, scriptName.value(), scriptCode->View(), modUID);
}

std::shared_ptr<ScriptLoader::Module> ScriptLoader::LoadImpl(ScriptID scriptID, std::string_view scriptName, std::string_view scriptCode, std::string_view modUID)
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

	auto &&result = GetScriptEngine().LoadFunction(std::string(scriptName), scriptCode);
	if (!result.valid())
	{
		_log->Debug("Failed to load script <{}>\"{}\": {}", scriptID, scriptName, result.get<sol::error>().what());

		return nullptr;
	}

	auto &&function = result.get<sol::protected_function>();
	auto &&module = std::make_shared<Module>(scriptID, function);
	auto &&[it, inserted] = _scriptModules.try_emplace(scriptID, module);
	assert(inserted);

	if (GetScriptProvider().IsStaticScript(scriptName))
	{
		module->RawLoad(*this);
		_log->Trace("Raw loaded script <{}>\"{}\"", scriptID, scriptName);
	}
	else
	{
		bool sandboxed = false;
		if (modUID != emptyString)
		{
			auto &&mod = GetModManager().FindLoadedMod(modUID);
			if (!mod.expired() && mod.lock()->GetConfig().scripts.sandbox)
			{
				sandboxed = true;
			}
		}
		GetScriptEngine().InitializeScript(scriptID, scriptName, modUID, sandboxed, function);
		_log->Trace("Lazy loaded script <{}>\"{}\"", scriptID, scriptName);
	}

	_onLoadedScript(scriptID);

	return it->second;
}

void ScriptLoader::UnloadAllScripts()
{
	auto &&scriptProvider = GetScriptProvider();

	for (auto &&it : _scriptModules)
	{
		if (!scriptProvider.GetScriptNameByID(it.first))
		{
			UnloadScript(it.first);
		}
	}
}

std::vector<ScriptID> ScriptLoader::UnloadScript(ScriptID scriptID)
{
	std::vector<ScriptID> unloadedScripts{};
	UnloadImpl(scriptID, unloadedScripts);
	return std::move(unloadedScripts);
}

std::vector<ScriptID> ScriptLoader::UnloadScriptsBy(std::string_view modUID)
{
	std::vector<ScriptID> unloadedScripts{};

	std::vector<ScriptID> unloadingScripts{};
	auto &&scriptProvider = GetScriptProvider();
	for (auto &&[scriptID, _] : _scriptModules)
	{
		if (scriptProvider.GetScriptModUID(scriptID) == modUID)
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

std::vector<ScriptID> ScriptLoader::UnloadInvalidScripts()
{
	// TODO
	throw std::runtime_error("NOT IMPLEMENT YET");
	// std::vector<ScriptID> unloadedScripts{};
	// return std::move(unloadedScripts);
}

void ScriptLoader::UnloadImpl(ScriptID scriptID, std::vector<ScriptID> &unloadedScripts)
{
	auto &&scriptName = GetScriptProvider().GetScriptNameByID(scriptID);
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

	GetScriptEngine().DeinitializeScript(scriptID, scriptName.value());
	_onUnloadScript(scriptID);

	for (auto &&dependency : GetScriptDependencies(scriptID))
	{
		UnloadScript(dependency);
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

void ScriptLoader::HotReloadScripts(const std::vector<ScriptID> &scriptIDs)
{
	_log->Debug("Hot reloading scripts ...");

	GetEngine().SetLoadingInfo(Engine::LoadingInfoArgs{
	    .title = "Loading scripts",
	    .description = "",
	    .progressValue = 0.0f,
	    .progressTotal = std::float_t(scriptIDs.size()),
	});

	if (_log->CanTrace())
	{
		auto &&scriptProvider = GetScriptProvider();
		for (auto scriptID : scriptIDs)
		{
			auto &&scriptName = scriptProvider.GetScriptNameByID(scriptID);
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
			_log->Trace("Fully loaded script \"{}\"", GetScriptProvider().GetScriptNameByID(scriptID).value());
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

		std::ranges::sort(temp, [](const auto &a, const auto &b) constexpr
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
