/**
 * @file ScriptModule.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025-08-15
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "program/Context.hpp"
#include "util/Definitions.hpp"
#include "util/Log.hpp"

#include "sol/table.hpp"

namespace tudov
{
	struct IScriptLoader;

	/**
	 * @brief A wrapper of script's module.
	 */
	struct IScriptModule
	{
		/**
		 * Check if current script module is valid.
		 */
		virtual bool IsValid() const = 0;

		/**
		 * Check if current script module is lazy loaded, which has a already been a wrapper of module's return table.
		 */
		virtual bool IsLazyLoaded() const = 0;

		/**
		 * Check if current script module is fully loaded (or raw loaded).
		 */
		virtual bool IsFullyLoaded() const = 0;

		/**
		 * Get module's wrapperred table. Note: it is not the original module table, wrapperred one instead.
		 */
		virtual const sol::table &GetTable() = 0;

		/**
		 * Raw load means no sandbox, no environment overriding, just load this script from pure globals.
		 */
		virtual const sol::table &RawLoad(IScriptLoader &scriptLoader) = 0;

		/**
		 * TODO
		 */
		virtual const sol::table &LazyLoad(IScriptLoader &scriptLoader) = 0;

		/**
		 * TODO
		 */
		virtual const sol::table &FullLoad(IScriptLoader &scriptLoader) = 0;
	};

	struct ScriptModule : public IScriptModule, public IContextProvider, private ILogProvider
	{
	  protected:
		static Context *_parentContext;
		static std::weak_ptr<Log> _parentLog;

	  protected:
		ScriptID _scriptID;
		sol::protected_function _func;
		bool _fullyLoaded;
		sol::table _table;

	  public:
		explicit ScriptModule();
		explicit ScriptModule(ScriptID scriptID, const sol::protected_function &func);

		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		bool IsValid() const override;
		bool IsLazyLoaded() const override;
		bool IsFullyLoaded() const override;

		const sol::table &GetTable() override;
		const sol::table &RawLoad(IScriptLoader &scriptLoader) override;
		const sol::table &LazyLoad(IScriptLoader &scriptLoader) override;
		const sol::table &FullLoad(IScriptLoader &scriptLoader) override;
	};
} // namespace tudov