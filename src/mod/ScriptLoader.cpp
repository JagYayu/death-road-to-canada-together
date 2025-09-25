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
#include "event/CoreEventsData.hpp"
#include "event/EventHandleKey.hpp"
#include "event/RuntimeEvent.hpp"
#include "misc/Text.hpp"
#include "mod/ModManager.hpp"
#include "mod/ScriptEngine.hpp"
#include "mod/ScriptModule.hpp"
#include "mod/ScriptProvider.hpp"
#include "program/Engine.hpp"
#include "program/Tudov.hpp"
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
      _scriptReversedDependencies(),
      _flags(),
      _parseErrorScripts(),
      _scriptProviderVersion()
{
}

ScriptLoader::~ScriptLoader() noexcept
{
	UnloadAllScripts();
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
	if (_parseErrorScripts.contains(scriptID))
	{
		return true;
	}

	auto it = _scriptModules.find(scriptID);
	return it == _scriptModules.end() ? false : it->second->HasLoadError();
}

void ScriptLoader::MarkScriptLoadError(ScriptID scriptID)
{
	auto it = _scriptModules.find(scriptID);
	if (it == _scriptModules.end()) [[unlikely]]
	{
		throw std::runtime_error("Script does not exist");
	}

	it->second->MarkLoadError();
}

std::vector<ScriptID> ScriptLoader::CollectErrorScripts() const noexcept
{
	CheckScriptProvider();

	std::vector<ScriptID> scriptIDs{};

	for (auto scriptID : _parseErrorScripts)
	{
		scriptIDs.emplace_back(scriptID);
	}

	for (auto [scriptID, scriptModule] : _scriptModules)
	{
		if (scriptModule->HasLoadError())
		{
			TE_ASSERT(!_parseErrorScripts.contains(scriptID));

			scriptIDs.emplace_back(scriptID);
		}
	}

	return scriptIDs;
}

void ScriptLoader::CheckScriptProvider() const noexcept
{
	const IScriptProvider &provider = GetScriptProvider();
	if (provider.GetVersionID() != _scriptProviderVersion) [[unlikely]]
	{
		std::erase_if(_parseErrorScripts, [&provider](ScriptID scriptID) -> bool
		{
			return !provider.IsValidScript(scriptID);
		});

		_scriptProviderVersion = provider.GetVersionID();
	}
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
		TE_WARN("Attempt to link invalid scripts to reverse dependency map: <{}>{} <- <{}>{}",
		        source, sourceName.has_value() ? sourceName->data() : "#INVALID#",
		        target, targetName.has_value() ? targetName->data() : "#INVALID#");

		return;
	}
	if (scriptProvider.IsStaticScript(source) || scriptProvider.IsStaticScript(target)) [[unlikely]]
	{
		auto &&sourceName = scriptProvider.GetScriptNameByID(source);
		auto &&targetName = scriptProvider.GetScriptNameByID(target);
		TE_WARN("Attempt to link static scripts to reverse dependency map: <{}>{} <- <{}>{}",
		        source, sourceName.has_value() ? sourceName->data() : "#INVALID#",
		        target, targetName.has_value() ? targetName->data() : "#INVALID#");

		return;
	}

	auto &&dependencies = _scriptReversedDependencies[target];
	dependencies.insert(source);
	// _scriptDependencyGraph.AddLink(from, to);

	TE_TRACE("Link scripts to reverse dependency map: <{}>{} <- <{}>{}",
	         source, scriptProvider.GetScriptNameByID(source)->data(),
	         target, scriptProvider.GetScriptNameByID(target)->data());
}

void ScriptLoader::LoadAllScripts()
{
	TE_DEBUG("Loading provided scripts ...");

	UnloadAllScripts();

	IScriptProvider &scriptProvider = GetScriptProvider();

	GetEngine().SetLoadingInfo(Engine::LoadingInfoArgs{
	    .title = "Loading scripts",
	    .description = "",
	    .progressValue = 0.0f,
	    .progressTotal = std::float_t(scriptProvider.GetCount()),
	});

	_onPreLoadAllScripts();

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

	TE_DEBUG("Loaded all provided scripts");
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
		TE_WARN("Attempt to load invalid script <{}>{}: script code not found, textID: {}",
		        scriptID, scriptName->data(), scriptProvider.GetScriptTextID(scriptID));
		return nullptr;
	}

	std::string_view modUID = scriptProvider.GetScriptModUID(scriptID);
	return LoadImpl(scriptID, scriptName.value(), scriptCode->View(), modUID);
}

std::shared_ptr<ScriptModule> ScriptLoader::LoadImpl(ScriptID scriptID, std::string_view scriptName, std::string_view scriptCode, std::string_view modUID)
{
	if (auto it = _scriptModules.find(scriptID); it != _scriptModules.end())
	{
		return it->second;
	}

	TE_DEBUG("Loading script <{}>{} ...", scriptID, scriptName);

	sol::load_result result = GetScriptEngine().LoadFunction(std::format("<{}>{}", scriptID, scriptName), scriptCode);
	if (result.valid())
	{
		auto &&function = result.get<sol::protected_function>();
		auto &&scriptModule = std::make_shared<ScriptModule>(*this, scriptID, function);
		auto &&[it, inserted] = _scriptModules.try_emplace(scriptID, scriptModule);
		TE_ASSERT(inserted);

		if (GetScriptProvider().IsStaticScript(scriptName))
		{
			scriptModule->RawLoad();
			TE_TRACE("Raw loaded script <{}>{}", scriptID, scriptName);
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
			TE_TRACE("Lazy loaded script <{}>{}", scriptID, scriptName);
		}

		_onLoadedScript(scriptID, scriptName);

		_parseErrorScripts.erase(scriptID);

		return it->second;
	}
	else
	{
		sol::error err = result;
		TE_ERROR("Failed to parse script <{}>{}: {}", scriptID, scriptName, err.what());

		auto &&scriptModule = std::make_shared<ScriptModule>(*this, scriptID, sol::protected_function());
		auto &&[_, inserted] = _scriptModules.try_emplace(scriptID, scriptModule);
		TE_ASSERT(inserted);

		_onFailedLoadScript(scriptID, scriptName, std::string_view(err.what()));

		_parseErrorScripts.emplace(scriptID);

		return nullptr;
	}
}

void ScriptLoader::UnloadAllScripts()
{
	if (_scriptModules.empty())
	{
		return;
	}

	TE_DEBUG("Unloading loaded scripts ...");

	IScriptProvider &scriptProvider = GetScriptProvider();

	std::vector<ScriptID> keys{};
	keys.reserve(_scriptModules.size());
	for (auto [scriptID, _] : _scriptModules)
	{
		if (scriptProvider.GetScriptNameByID(scriptID).has_value())
		{
			keys.emplace_back(scriptID);
		}
	}

	for (ScriptID scriptID : keys)
	{
		UnloadScript(scriptID);
	}

	_scriptModules.clear();
	_parseErrorScripts.clear();

	TE_DEBUG("Unloaded loaded scripts ...");
}

std::vector<ScriptID> ScriptLoader::UnloadScript(ScriptID scriptID)
{
	std::vector<ScriptID> unloadedScripts{};
	UnloadImpl(scriptID, &unloadedScripts);
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
		UnloadImpl(scriptID, &unloadedScripts);
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

void ScriptLoader::UnloadImpl(ScriptID scriptID, std::vector<ScriptID> *unloadedScripts)
{
	if (!_scriptModules.contains(scriptID))
	{
		TE_TRACE("Script <{}> has already unloaded", scriptID);

		return;
	}

	auto optScriptName = GetScriptProvider().GetScriptNameByID(scriptID);
	if (!optScriptName.has_value()) [[unlikely]]
	{
		TE_WARN("Attempt to unload invalid script <{}> ...", scriptID);

		return;
	}
	std::string_view scriptName = optScriptName.value();

	TE_TRACE("Unloading script <{}>{} ...", scriptID, scriptName);

	{
		EventScriptUnloadData data{
		    .scriptID = scriptID,
		    .scriptName = scriptName,
		    .module = _scriptModules.at(scriptID)->GetTable(),
		};
		GetEventManager().GetCoreEvents().ScriptUnload().Invoke(&data, EventHandleKey(scriptName));
	}

	_onUnloadScript(scriptID, scriptName);

	TE_ASSERT(_scriptModules.erase(scriptID));

	if (unloadedScripts != nullptr)
	{
		for (ScriptID dependency : GetScriptDependencies(scriptID))
		{
			UnloadScript(dependency);
			unloadedScripts->emplace_back(dependency);
		}
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
			_log->Trace("<{}>{}", scriptID, optScriptName.has_value() ? optScriptName->data() : "#INVALID#");
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
				TE_TRACE("Fully loading script <{}>{}", scriptID, GetScriptProvider().GetScriptNameByID(scriptID).value());
				scriptModule->FullLoad();
				TE_TRACE("Fully loaded script <{}>{}", scriptID, GetScriptProvider().GetScriptNameByID(scriptID).value());
			}
			catch (std::exception &e)
			{
				TE_TRACE("Error full load script <{}>{}: {}", scriptID, GetScriptProvider().GetScriptNameByID(scriptID).value(), e.what());
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
