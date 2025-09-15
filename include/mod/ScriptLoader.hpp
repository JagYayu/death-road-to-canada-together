/**
 * @file mod/ScriptLoader.hpp
 * @author JagYayu
 * @brief Lua Script Loader
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "ScriptProvider.hpp"
#include "event/DelegateEvent.hpp"
#include "program/EngineComponent.hpp"
#include "system/Log.hpp"
#include "util/Definitions.hpp"
#include "util/Micros.hpp"

#include <sol/table.hpp>

#include <memory>
#include <optional>
#include <set>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace tudov
{
	class ScriptEngine;
	class ScriptError;
	struct IScriptModule;
	class ScriptModule;

	/**
	 * Manage script's modules / functions.
	 * Resolve dependency issues.
	 * Mainly used by ModManager.
	 */
	struct IScriptLoader : public IEngineComponent
	{
		virtual ~IScriptLoader() noexcept = default;

		virtual DelegateEvent<> &GetOnPreLoadAllScripts() noexcept = 0;

		virtual DelegateEvent<> &GetOnPostLoadAllScripts() noexcept = 0;

		virtual DelegateEvent<const std::vector<ScriptID> &> &GetOnPreHotReloadScripts() noexcept = 0;

		virtual DelegateEvent<const std::vector<ScriptID> &> &GetOnPostHotReloadScripts() noexcept = 0;

		/*
		 * Should be called right after a script was `RawLoaded` or `LazyLoaded`.
		 */
		virtual DelegateEvent<ScriptID, std::string_view> &GetOnLoadedScript() = 0;
		/*
		 * Should be called before a script was unloaded.
		 */
		virtual DelegateEvent<ScriptID, std::string_view> &GetOnUnloadScript() = 0;
		/*
		 * Should be called right after a script was failed to `RawLoaded` or `LazyLoaded`.
		 */
		virtual DelegateEvent<ScriptID, std::string_view, std::string_view> &GetOnFailedLoadScript() = 0;

		/**
		 * Check if the script module with specific id exists.
		 * @param[in]  scriptID @see ScriptID
		 * @return true or false
		 */
		virtual bool IsScriptExists(ScriptID scriptID) noexcept = 0;

		/**
		 * @see `IScriptModule::IsValid`
		 */
		virtual bool IsScriptValid(ScriptID scriptID) noexcept = 0;

		/**
		 * @see `IScriptModule::IsLazyLoaded`
		 */
		virtual bool IsScriptLazyLoaded(ScriptID scriptID) noexcept = 0;

		/**
		 * @see `IScriptModule::IsFullyLoaded`
		 */
		virtual bool IsScriptFullyLoaded(ScriptID scriptID) noexcept = 0;

		/**
		 * @see `IScriptModule::HasLoadError`
		 */
		virtual bool IsScriptHasError(ScriptID scriptID) noexcept = 0;

		/**
		 * Mark a script has loadtime error.
		 */
		virtual void MarkScriptLoadError(ScriptID scriptID) = 0;

		/**
		 * Collect a vector of script id that has load error. Result scripts are unsorted.
		 */
		virtual std::vector<ScriptID> CollectErrorScripts() const noexcept = 0;

		/**
		 * Get current loading script id. More preciously, is the script that in "FullLoad" (or "RawLoad"), but not in "LazyLoad" since its a wrapper process and didn't run the content of script.
		 * @return Current loading script id
		 */
		virtual ScriptID GetLoadingScriptID() const noexcept = 0;

		/**
		 * Get current loading script id's name/
		 * @see ScriptID ScriptLoader::GetLoadingScriptID() const noexcept override;
		 * @return Current loading script name
		 */
		virtual std::optional<std::string_view> GetLoadingScriptName() const noexcept = 0;

		/**
		 * Get all scripts that has dependency on it.
		 * Result vector is unordered, exclude parameter itself.
		 */
		virtual std::vector<ScriptID> GetScriptDependencies(ScriptID scriptID) const = 0;

		/**
		 * Link script dependency, e.g. "A.lua" has script `require "B.lua"`, then "B.lua"'s hot-reload procession will also affect "A.lua".
		 * I know its a bad naming yeah, just ignore the "Reverse" part. Consider it as a "AddScriptDependency" function.
		 * @param[in]  source Source script id that pointer to target script.
		 * @param[in]  target Target script id that was pointred by source script.
		 */
		virtual void AddReverseDependency(ScriptID source, ScriptID target) = 0;

		virtual void LoadAllScripts() = 0;

		virtual void UnloadAllScripts() = 0;

		/**
		 * Try load script's module, do nothing if already loaded.
		 */
		virtual std::shared_ptr<IScriptModule> Load(ScriptID scriptID) = 0;

		/**
		 * Similar to `Load`, try parse `scriptName` to `scriptID`.
		 */
		virtual std::shared_ptr<IScriptModule> Load(std::string_view scriptName) = 0;

		/**
		 * Try unload script's module, it also unload all scripts that depend on it, so it returns a vector of script ids.
		 * Result vector is unordered, exclude parameter itself.
		 */
		virtual std::vector<ScriptID> UnloadScript(ScriptID scriptID) = 0;

		/**
		 * Try unload scripts' modules by modUID, this will remove all scripts that are from specific mod and should be called at mod side.
		 */
		virtual std::vector<ScriptID> UnloadScriptsBy(std::string_view modUID) = 0;

		/**
		 * Try unload invalid scripts' modules that not provided by script provider.
		 */
		virtual std::vector<ScriptID> UnloadInvalidScripts() = 0;

		/**
		 * Hot reload a vector of scripts.
		 * Note that this is an internal implementation of the lua script system. To ensure the correctness of overloading,
		 * ensure that the passed vector of hot reload scripts contains all scripts that depend on them.
		 */
		virtual void HotReloadScripts(const std::vector<ScriptID> &scriptIDs) = 0;

		/**
		 * Process script full load operations.
		 */
		virtual void ProcessFullLoads() = 0;

		inline const DelegateEvent<> &GetOnPreLoadAllScripts() const noexcept
		{
			return const_cast<IScriptLoader *>(this)->GetOnPreLoadAllScripts();
		}

		inline const DelegateEvent<> &GetOnPostLoadAllScripts() const noexcept
		{
			return const_cast<IScriptLoader *>(this)->GetOnPostLoadAllScripts();
		}

		inline const DelegateEvent<const std::vector<ScriptID> &> &GetOnPreHotReloadScripts() const noexcept
		{
			return const_cast<IScriptLoader *>(this)->GetOnPreHotReloadScripts();
		}

		inline const DelegateEvent<const std::vector<ScriptID> &> &GetOnPostHotReloadScripts() const noexcept
		{
			return const_cast<IScriptLoader *>(this)->GetOnPostHotReloadScripts();
		}

		inline const DelegateEvent<ScriptID, std::string_view> &GetOnLoadedScript() const noexcept
		{
			return const_cast<IScriptLoader *>(this)->GetOnLoadedScript();
		}

		inline const DelegateEvent<ScriptID, std::string_view> &GetOnUnloadScript() const noexcept
		{
			return const_cast<IScriptLoader *>(this)->GetOnUnloadScript();
		}

		inline const DelegateEvent<ScriptID, std::string_view, std::string_view> &GetOnFailedLoadScript() const noexcept
		{
			return const_cast<IScriptLoader *>(this)->GetOnFailedLoadScript();
		}

		inline bool IsScriptExists(std::string_view scriptName) noexcept
		{
			return IsScriptExists(GetScriptIDByName(scriptName));
		}

		inline bool IsScriptValid(std::string_view scriptName) noexcept
		{
			return IsScriptValid(GetScriptIDByName(scriptName));
		}

		inline bool IsScriptLazyLoaded(std::string_view scriptName) noexcept
		{
			return IsScriptLazyLoaded(GetScriptIDByName(scriptName));
		}

		inline bool IsScriptFullyLoaded(std::string_view scriptName) noexcept
		{
			return IsScriptFullyLoaded(GetScriptIDByName(scriptName));
		}

		inline std::vector<ScriptID> UnloadScript(std::string_view scriptName)
		{
			return UnloadScript(GetScriptIDByName(scriptName));
		}

	  protected:
		TE_FORCEINLINE ScriptID GetScriptIDByName(std::string_view scriptName)
		{
			return GetScriptProvider().GetScriptIDByName(scriptName);
		}
	};

	class ScriptLoader : public IScriptLoader, private ILogProvider
	{
		friend LuaBindings;
		friend ScriptModule;

	  protected:
		Context &_context;
		std::shared_ptr<Log> _log;
		std::unordered_map<ScriptID, std::shared_ptr<ScriptModule>> _scriptModules;
		std::unordered_map<ScriptID, std::set<ScriptID>> _scriptReversedDependencies;

		// Set this value when a script module is doing whatever loads.
		ScriptID _loadingScript;
		/**
		 * Push scriptID whenever a script is doing full load, pop it after load done.
		 * It is used to detect which modules encountered circular dependency issue.
		 */
		std::vector<ScriptID> _scriptLoopLoadStack;

		DelegateEvent<> _onPreLoadAllScripts;
		DelegateEvent<> _onPostLoadAllScripts;
		DelegateEvent<const std::vector<ScriptID> &> _onPreHotReloadScripts;
		DelegateEvent<const std::vector<ScriptID> &> _onPostHotReloadScripts;
		DelegateEvent<ScriptID, std::string_view> _onLoadedScript;
		DelegateEvent<ScriptID, std::string_view> _onUnloadScript;
		DelegateEvent<ScriptID, std::string_view, std::string_view> _onFailedLoadScript;

		mutable std::unordered_set<ScriptID> _parseErrorScripts;
		mutable std::uint64_t _scriptProviderVersion;

	  public:
		explicit ScriptLoader(Context &context) noexcept;
		~ScriptLoader() noexcept override;

	  public:
		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		DelegateEvent<> &GetOnPreLoadAllScripts() noexcept override;
		DelegateEvent<> &GetOnPostLoadAllScripts() noexcept override;
		DelegateEvent<const std::vector<ScriptID> &> &GetOnPreHotReloadScripts() noexcept override;
		DelegateEvent<const std::vector<ScriptID> &> &GetOnPostHotReloadScripts() noexcept override;
		DelegateEvent<ScriptID, std::string_view> &GetOnLoadedScript() noexcept override;
		DelegateEvent<ScriptID, std::string_view> &GetOnUnloadScript() noexcept override;
		DelegateEvent<ScriptID, std::string_view, std::string_view> &GetOnFailedLoadScript() noexcept override;

		bool IsScriptExists(ScriptID scriptID) noexcept override;
		bool IsScriptValid(ScriptID scriptID) noexcept override;
		bool IsScriptLazyLoaded(ScriptID scriptID) noexcept override;
		bool IsScriptFullyLoaded(ScriptID scriptID) noexcept override;
		bool IsScriptHasError(ScriptID scriptID) noexcept override;
		void MarkScriptLoadError(ScriptID scriptID) override;
		std::vector<ScriptID> CollectErrorScripts() const noexcept override;
		ScriptID GetLoadingScriptID() const noexcept override;
		std::optional<std::string_view> GetLoadingScriptName() const noexcept override;
		std::vector<ScriptID> GetScriptDependencies(ScriptID scriptID) const override;
		void AddReverseDependency(ScriptID source, ScriptID target) override;

		void LoadAllScripts() override;
		void UnloadAllScripts() override;
		std::shared_ptr<IScriptModule> Load(ScriptID scriptID) override;
		std::shared_ptr<IScriptModule> Load(std::string_view scriptName) override;
		std::vector<ScriptID> UnloadScript(ScriptID scriptID) override;
		std::vector<ScriptID> UnloadScriptsBy(std::string_view namespace_) override;
		std::vector<ScriptID> UnloadInvalidScripts() override;
		void HotReloadScripts(const std::vector<ScriptID> &scriptIDs) override;
		void ProcessFullLoads() override;

	  protected:
		void CheckScriptProvider() const noexcept;
		std::shared_ptr<ScriptModule> LoadImpl(ScriptID scriptID, std::string_view scriptName, std::string_view code, std::string_view mod);
		void UnloadImpl(ScriptID scriptID, std::vector<ScriptID> &unloadedScripts);
		void PostLoadScripts();

	  private:
		void LuaAddReverseDependency(sol::object source, sol::object target) noexcept;
	};
} // namespace tudov
