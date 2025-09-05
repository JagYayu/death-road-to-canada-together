/**
 * @file mod/ScriptLoader.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "mod/ScriptLoader.hpp"

#include "event/CoreEvents.hpp"
#include "event/RuntimeEvent.hpp"
#include "misc/Text.hpp"
#include "mod/ModManager.hpp"
#include "mod/ScriptEngine.hpp"
#include "mod/ScriptModule.hpp"
#include "mod/ScriptProvider.hpp"
#include "program/Engine.hpp"
#include "resource/GlobalResourcesCollection.hpp"
#include "system/LogMicros.hpp"
#include "util/Definitions.hpp"
#include "util/StringUtils.hpp"

#include <sol/error.hpp>
#include <sol/forward.hpp>
#include <sol/types.hpp>

#include <corecrt_terminate.h>
#include <format>
#include <memory>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <unordered_set>
#include <vector>

using namespace tudov;

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
      _onFailedLoadScript(),
      _scriptReversedDependencies()
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

Log &ScriptLoader::GetLog() noexcept
{
	return *_log;
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

DelegateEvent<ScriptID, std::string_view> &ScriptLoader::GetOnLoadedScript() noexcept
{
	return _onLoadedScript;
}

DelegateEvent<ScriptID, std::string_view> &ScriptLoader::GetOnUnloadScript() noexcept
{
	return _onUnloadScript;
}

DelegateEvent<ScriptID, std::string_view, std::string_view> &ScriptLoader::GetOnFailedLoadScript() noexcept
{
	return _onFailedLoadScript;
}

bool ScriptLoader::IsScriptExists(ScriptID scriptID) noexcept
{
	return _scriptModules.contains(scriptID);
}

bool ScriptLoader::IsScriptValid(ScriptID scriptID) noexcept
{
	auto it = _scriptModules.find(scriptID);
	return it == _scriptModules.end() ? false : it->second->IsValid();
}

bool ScriptLoader::IsScriptLazyLoaded(ScriptID scriptID) noexcept
{
	auto it = _scriptModules.find(scriptID);
	return it == _scriptModules.end() ? false : it->second->IsLazyLoaded();
}

bool ScriptLoader::IsScriptFullyLoaded(ScriptID scriptID) noexcept
{
	auto it = _scriptModules.find(scriptID);
	return it == _scriptModules.end() ? false : it->second->IsFullyLoaded();
}

bool ScriptLoader::IsScriptHasError(ScriptID scriptID) noexcept
{
	auto it = _scriptModules.find(scriptID);
	return it == _scriptModules.end() ? false : it->second->HasLoadError();
}

ScriptID ScriptLoader::GetLoadingScriptID() const noexcept
{
	return _loadingScript;
}

std::optional<std::string_view> ScriptLoader::GetLoadingScriptName() const noexcept
{
	return GetScriptProvider().GetScriptNameByID(_loadingScript);
}

void GetScriptDependenciesImpl(const std::unordered_map<ScriptID, std::set<ScriptID>> &scriptDependencies, ScriptID scriptID, std::vector<ScriptID> &sources, std::unordered_set<ScriptID> &visited)
{
	if (visited.contains(scriptID))
	{
		return;
	}
	visited.emplace(scriptID);

	auto it = scriptDependencies.find(scriptID);
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
	std::vector<ScriptID> sources{};
	std::unordered_set<ScriptID> visited{};
	GetScriptDependenciesImpl(_scriptReversedDependencies, scriptID, sources, visited);
	return sources;
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
		TE_WARN("Attempt to link invalid scripts to reverse dependency map: <{}>\"{}\" <- <{}>\"{}\"",
		        source, sourceName.has_value() ? sourceName->data() : "#INVALID#",
		        target, targetName.has_value() ? targetName->data() : "#INVALID#");

		return;
	}
	if (scriptProvider.IsStaticScript(source) || scriptProvider.IsStaticScript(target)) [[unlikely]]
	{
		auto &&sourceName = scriptProvider.GetScriptNameByID(source);
		auto &&targetName = scriptProvider.GetScriptNameByID(target);
		TE_WARN("Attempt to link static scripts to reverse dependency map: <{}>\"{}\" <- <{}>\"{}\"",
		        source, sourceName.has_value() ? sourceName->data() : "#INVALID#",
		        target, targetName.has_value() ? targetName->data() : "#INVALID#");

		return;
	}

	auto &&dependencies = _scriptReversedDependencies[target];
	dependencies.insert(source);
	// _scriptDependencyGraph.AddLink(from, to);

	TE_TRACE("Link scripts to reverse dependency map: <{}>\"{}\" <- <{}>\"{}\"",
	         source, scriptProvider.GetScriptNameByID(source)->data(),
	         target, scriptProvider.GetScriptNameByID(target)->data());
}

void ScriptLoader::LoadAllScripts()
{
	TE_DEBUG("{}", "Loading provided scripts ...");

	IScriptProvider &scriptProvider = GetScriptProvider();

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

	TextResources &textResources = GetGlobalResourcesCollection().GetTextResources();

	auto &&count = scriptProvider.GetCount();
	for (const auto &entry : scriptProvider)
	{
		std::shared_ptr<Text> code = textResources.GetResource(entry.textID);
		TE_ASSERT(code != nullptr);
		LoadImpl(entry.scriptID, entry.name, code->View(), entry.modUID);
	}

	ProcessFullLoads();

	_onPostLoadAllScripts();

	PostLoadScripts();

	TE_DEBUG("{}", "Loaded provided scripts");
}

std::shared_ptr<IScriptModule> ScriptLoader::Load(std::string_view scriptName)
{
	ScriptID scriptID = GetScriptProvider().GetScriptIDByName(scriptName);
	if (!scriptID)
	{
		return nullptr;
	}
	return Load(scriptID);
}

std::shared_ptr<IScriptModule> ScriptLoader::Load(ScriptID scriptID)
{
	if (!scriptID) [[unlikely]]
	{
		TE_WARN("Attempt to load invalid script <{}>: id is 0", scriptID);
		return nullptr;
	}
	IScriptProvider &scriptProvider = GetScriptProvider();
	auto scriptName = scriptProvider.GetScriptNameByID(scriptID);
	if (!scriptName.has_value()) [[unlikely]]
	{
		TE_WARN("Attempt to load invalid script <{}>: not provided", scriptID);
		return nullptr;
	}

	const auto &scriptCode = scriptProvider.GetScriptCode(scriptID);
	if (scriptCode == nullptr) [[unlikely]]
	{
		TE_WARN("Attempt to load invalid script <{}>\"{}\": script code not found, textID: {}",
		        scriptID, scriptName->data(), scriptProvider.GetScriptTextID(scriptID));
		return nullptr;
	}

	std::string_view modUID = scriptProvider.GetScriptModUID(scriptID);
	return LoadImpl(scriptID, scriptName.value(), scriptCode->View(), modUID);
}

std::shared_ptr<ScriptModule> ScriptLoader::LoadImpl(ScriptID scriptID, std::string_view scriptName, std::string_view scriptCode, std::string_view modUID)
{
	{
		auto it = _scriptModules.find(scriptID);
		if (it != _scriptModules.end())
		{
			return it->second;
		}
	}

	TE_DEBUG("Loading script <{}>\"{}\" ...", scriptID, scriptName);

	sol::load_result result = GetScriptEngine().LoadFunction(std::format("<{}>\"{}\"", scriptID, scriptName), scriptCode);
	if (result.valid())
	{
		auto &&function = result.get<sol::protected_function>();
		auto &&scriptModule = std::make_shared<ScriptModule>(scriptID, function);
		auto &&[it, inserted] = _scriptModules.try_emplace(scriptID, scriptModule);
		TE_ASSERT(inserted);

		if (GetScriptProvider().IsStaticScript(scriptName))
		{
			scriptModule->RawLoad(*this);
			TE_TRACE("Raw loaded script <{}>\"{}\"", scriptID, scriptName);
		}
		else
		{
			bool sandboxed = false;
			if (modUID != emptyString)
			{
				auto &&mod = GetModManager().FindLoadedMod(modUID);
				if (mod != nullptr && mod->GetConfig().scripts.sandbox)
				{
					sandboxed = true;
				}
			}

			GetScriptEngine().InitializeScript(scriptID, scriptName, modUID, sandboxed, function);
			TE_TRACE("Lazy loaded script <{}>\"{}\"", scriptID, scriptName);
		}

		_onLoadedScript(scriptID, scriptName);

		return it->second;
	}
	else
	{
		sol::error err = result;
		TE_ERROR("Failed to load script <{}>\"{}\": {}", scriptID, scriptName, err.what());

		auto &&scriptModule = std::make_shared<ScriptModule>(scriptID, sol::protected_function());
		auto &&[_, inserted] = _scriptModules.try_emplace(scriptID, scriptModule);
		TE_ASSERT(inserted);

		_onFailedLoadScript(scriptID, scriptName, std::string_view(err.what()));

		return nullptr;
	}
}

void ScriptLoader::UnloadAllScripts()
{
	IScriptProvider &scriptProvider = GetScriptProvider();

	for (auto it : _scriptModules)
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
	return unloadedScripts;
}

std::vector<ScriptID> ScriptLoader::UnloadScriptsBy(std::string_view modUID)
{
	std::vector<ScriptID> unloadedScripts{};

	std::vector<ScriptID> unloadingScripts{};
	IScriptProvider &scriptProvider = GetScriptProvider();
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
	auto scriptName = GetScriptProvider().GetScriptNameByID(scriptID);
	if (!scriptName.has_value()) [[unlikely]]
	{
		TE_WARN("Attempt to unload invalid script <{}> ...", scriptID);

		return;
	}

	TE_TRACE("Unloading script <{}>\"{}\" ...", scriptID, scriptName->data());

	if (!_scriptModules.erase(scriptID))
	{
		TE_TRACE("{}", "Already unloaded");
		return;
	}

	_onUnloadScript(scriptID, scriptName.value());

	for (ScriptID dependency : GetScriptDependencies(scriptID))
	{
		UnloadScript(dependency);
		unloadedScripts.emplace_back(dependency);
	}

	_scriptReversedDependencies.erase(scriptID);
	for (auto &&[_, dependencies] : _scriptReversedDependencies)
	{
		dependencies.erase(scriptID);
	}

	TE_TRACE("{}", "Unloaded");
}

void ScriptLoader::HotReloadScripts(const std::vector<ScriptID> &scriptIDs)
{
	TE_DEBUG("{}", "Hot reloading scripts ...");

	GetEngine().SetLoadingInfo(Engine::LoadingInfoArgs{
	    .title = "Loading scripts",
	    .description = "",
	    .progressValue = 0.0f,
	    .progressTotal = std::float_t(scriptIDs.size()),
	});

	if (_log->CanTrace())
	{
		IScriptProvider &scriptProvider = GetScriptProvider();
		for (auto scriptID : scriptIDs)
		{
			auto optScriptName = scriptProvider.GetScriptNameByID(scriptID);
			_log->Trace("<{}>\"{}\"", scriptID, optScriptName.has_value() ? optScriptName->data() : "#INVALID#");
		}
	}

	_onPreHotReloadScripts(scriptIDs);

	for (auto scriptID : scriptIDs)
	{
		if (IsScriptFullyLoaded(scriptID))
		{
			UnloadScript(scriptID);
		}
		Load(scriptID);
	}
	ProcessFullLoads();

	_onPostHotReloadScripts(scriptIDs);

	PostLoadScripts();

	TE_DEBUG("{}", "Hot reloaded scripts");
}

void ScriptLoader::ProcessFullLoads()
{
	TE_DEBUG("{}", "Processing full loads ...");

	for (auto &&[scriptID, scriptModule] : _scriptModules)
	{
		if (!scriptModule->IsFullyLoaded())
		{
			try
			{
				TE_TRACE("Fully loading script \"{}\"", GetScriptProvider().GetScriptNameByID(scriptID).value());
				scriptModule->FullLoad(*this);
				TE_TRACE("Fully loaded script \"{}\"", GetScriptProvider().GetScriptNameByID(scriptID).value());
			}
			catch (std::exception &e)
			{
				TE_TRACE("Error full load script \"{}\": {}", GetScriptProvider().GetScriptNameByID(scriptID).value(), e.what());
			}
		}
	}

	TE_DEBUG("{}", "Processed full loads");
}

void ScriptLoader::PostLoadScripts()
{
	GetEventManager().GetCoreEvents().ScriptsLoaded().Invoke();

	Log::CleanupExpired();
}

void ScriptLoader::LuaAddReverseDependency(sol::object source, sol::object target) noexcept
{
	auto source_ = static_cast<ScriptID>(source.as<double_t>());
	auto target_ = static_cast<ScriptID>(target.as<double_t>());

	AddReverseDependency(source_, target_);
}
