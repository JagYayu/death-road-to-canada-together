/**
 * @file mod/ScriptModule.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "program/Context.hpp"
#include "system/Log.hpp"
#include "util/Definitions.hpp"

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
		 * Check if the script module is valid.
		 * This only checks whether the script module's function is valid, that is, whether the code is parsed properly.
		 */
		virtual bool IsValid() const noexcept = 0;

		/**
		 * Check if current script module is lazy loaded, which has a already been a wrapper of module's return table.
		 */
		virtual bool IsLazyLoaded() const noexcept = 0;

		/**
		 * Check if current script module is fully loaded (or raw loaded).
		 */
		virtual bool IsFullyLoaded() const noexcept = 0;

		/**
		 * Check if the script module has load error.
		 */
		virtual bool HasLoadError() const noexcept = 0;

		virtual void MarkLoadError() noexcept = 0;

		/**
		 * Get module's wrapperred table. Note: it is not the original module table, wrapperred one instead.
		 */
		virtual sol::table &GetTable() = 0;

		/**
		 * Raw load means no sandbox, no environment overriding, just load this script from pure globals.
		 */
		virtual sol::table &RawLoad(IScriptLoader &scriptLoader) = 0;

		/**
		 * TODO
		 */
		virtual sol::table &LazyLoad(IScriptLoader &scriptLoader) = 0;

		/**
		 * TODO
		 */
		virtual sol::table &FullLoad(IScriptLoader &scriptLoader) = 0;

		TE_FORCEINLINE const sol::table &GetTable() const
		{
			return const_cast<IScriptModule *>(this)->GetTable();
		}
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
		bool _hasError;
		sol::table _table;

	  public:
		explicit ScriptModule();
		explicit ScriptModule(ScriptID scriptID, const sol::protected_function &func);

		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		bool IsValid() const noexcept override;
		bool IsLazyLoaded() const noexcept override;
		bool IsFullyLoaded() const noexcept override;
		bool HasLoadError() const noexcept override;
		void MarkLoadError() noexcept override;

		sol::table &GetTable() override;
		sol::table &RawLoad(IScriptLoader &scriptLoader) override;
		sol::table &LazyLoad(IScriptLoader &scriptLoader) override;
		sol::table &FullLoad(IScriptLoader &scriptLoader) override;
	};
} // namespace tudov