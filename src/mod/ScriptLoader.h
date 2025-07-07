#pragma once

#include "ScriptProvider.h"
#include "event/DelegateEvent.hpp"
#include "program/IEngineComponent.h"
#include "util/Defs.h"
#include "util/DependencyGraph.hpp"
#include "util/Log.h"
#include "util/StringUtils.hpp"

#include <optional>
#include <sol/table.hpp>

#include <map>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace tudov
{
	class ScriptEngine;

	struct IScriptLoader : public IEngineComponent
	{
		struct IModule
		{
			virtual bool IsLazyLoaded() const = 0;
			virtual bool IsFullyLoaded() const = 0;

			virtual const sol::table &GetTable() = 0;
			virtual const sol::table &RawLoad(IScriptLoader &scriptLoader) = 0;
			virtual const sol::table &LazyLoad(IScriptLoader &scriptLoader) = 0;
			virtual const sol::table &FullLoad(IScriptLoader &scriptLoader) = 0;
		};

		virtual DelegateEvent<> GetOnPreLoadAllScripts() noexcept = 0;
		virtual DelegateEvent<> GetOnPostLoadAllScripts() noexcept = 0;
		virtual DelegateEvent<const std::vector<ScriptID> &> GetOnPreHotReloadScripts() noexcept = 0;
		virtual DelegateEvent<const std::vector<ScriptID> &> GetOnPostHotReloadScripts() noexcept = 0;
		/*
		 * Should be called right after a script was `RawLoaded` or `LazyLoaded`.
		 */
		virtual DelegateEvent<ScriptID> GetOnLoadedScript() = 0;
		/*
		 * Should be called before a script was unloaded.
		 */
		virtual DelegateEvent<ScriptID> GetOnUnloadScript() = 0;

		virtual ScriptID GetLoadingScriptID() const noexcept = 0;
		virtual std::optional<std::string_view> GetLoadingScriptName() const noexcept = 0;

		virtual std::vector<ScriptID> GetDependencies(ScriptID scriptID) const = 0;
		virtual void AddReverseDependency(ScriptID source, ScriptID target) = 0;

		virtual void LoadAll() = 0;
		virtual void UnloadAll() = 0;
		/*
		 * Try load script's module, do nothing if already loaded.
		 */
		virtual std::shared_ptr<IScriptLoader::IModule> Load(ScriptID scriptID) = 0;
		/*
		 * Similar to `Load`, try parse `scriptName` to `scriptID`.
		 */
		virtual std::shared_ptr<IScriptLoader::IModule> Load(std::string_view scriptName) = 0;
		/*
		 * Try unload script's module, it also uninstall all scripts that depend on it, return a vector of script ids.
		 */
		virtual std::vector<ScriptID> Unload(ScriptID scriptID) = 0;
		/*
		 * Hot reload a vector of scripts.
		 */
		virtual void HotReload(const std::vector<ScriptID> &scriptIDs) = 0;
		virtual void ProcessFullLoads() = 0;

		virtual bool HasAnyLoadError() const noexcept = 0;
		virtual std::vector<std::string> GetLoadErrors() noexcept = 0;
	};

	class ScriptLoader : public IScriptLoader
	{
	  private:
		struct Module : IModule
		{
		  private:
			ScriptID _scriptID;
			sol::protected_function _func;
			bool _fullyLoaded;
			sol::table _table;

		  public:
			explicit Module();
			explicit Module(ScriptID scriptID, const sol::protected_function &func);

			bool IsLazyLoaded() const override;
			bool IsFullyLoaded() const override;

			const sol::table &GetTable() override;
			const sol::table &RawLoad(IScriptLoader &scriptLoader) override;
			const sol::table &LazyLoad(IScriptLoader &scriptLoader) override;
			const sol::table &FullLoad(IScriptLoader &scriptLoader) override;
		};

	  public:
		using ScriptID = ScriptID;

	  protected:
		Context &_context;
		std::shared_ptr<Log> _log;
		std::unordered_map<ScriptID, std::shared_ptr<Module>> _scriptModules;
		std::unordered_map<ScriptID, std::unordered_set<ScriptID>> _scriptReverseDependencies;
		std::unordered_map<ScriptID, std::tuple<std::size_t, std::string>> _scriptErrors;
		std::optional<std::vector<std::string>> _scriptErrorsCache;

		// Set this value when a script is doing whatever loads.
		ScriptID _loadingScript;
		// Push scriptID whenever a script is doing full load, pop after done. Used to detect which modules has circular dependencies.
		std::vector<ScriptID> _scriptLoopLoadStack;

		DelegateEvent<> _onPreLoadAllScripts;
		DelegateEvent<> _onPostLoadAllScripts;
		DelegateEvent<const std::vector<ScriptID> &> _onPreHotReloadScripts;
		DelegateEvent<const std::vector<ScriptID> &> _onPostHotReloadScripts;
		DelegateEvent<ScriptID> _onLoadedScript;
		DelegateEvent<ScriptID> _onUnloadScript;

	  public:
		explicit ScriptLoader(Context &context) noexcept;
		~ScriptLoader() noexcept;

	  private:
		std::shared_ptr<Module> LoadImpl(ScriptID scriptID, std::string_view scriptName, std::string_view code, std::string_view mod);
		void UnloadImpl(ScriptID scriptID, std::vector<ScriptID> &unloadedScripts);

	  public:
		Context &GetContext() noexcept override;

		DelegateEvent<> GetOnPreLoadAllScripts() noexcept override;
		DelegateEvent<> GetOnPostLoadAllScripts() noexcept override;
		DelegateEvent<const std::vector<ScriptID> &> GetOnPreHotReloadScripts() noexcept override;
		DelegateEvent<const std::vector<ScriptID> &> GetOnPostHotReloadScripts() noexcept override;
		DelegateEvent<ScriptID> GetOnLoadedScript() noexcept override;
		DelegateEvent<ScriptID> GetOnUnloadScript() noexcept override;

		ScriptID GetLoadingScriptID() const noexcept override;
		std::optional<std::string_view> GetLoadingScriptName() const noexcept override;

		std::vector<ScriptID> GetDependencies(ScriptID scriptID) const override;
		void AddReverseDependency(ScriptID source, ScriptID target) override;

		void LoadAll() override;
		void UnloadAll() override;
		std::shared_ptr<IScriptLoader::IModule> Load(ScriptID scriptID) override;
		std::shared_ptr<IScriptLoader::IModule> Load(std::string_view scriptName) override;
		std::vector<ScriptID> Unload(ScriptID scriptID) override;
		void HotReload(const std::vector<ScriptID> &scriptIDs) override;
		void ProcessFullLoads() override;

		bool HasAnyLoadError() const noexcept override;
		std::vector<std::string> GetLoadErrors() noexcept override;
	};
} // namespace tudov
