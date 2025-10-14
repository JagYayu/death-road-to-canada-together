/**
 * @file Mod/ScriptModule.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "Program/Context.hpp"
#include "System/Log.hpp"
#include "Util/Definitions.hpp"

#include "sol/table.hpp"
#include "Util/Micros.hpp"
#include <memory>

namespace tudov
{
	struct IScriptLoader;

	/**
	 * @brief A wrapper of script's module.
	 */
	struct IScriptModule
	{
		virtual IScriptLoader &GetScriptLoader() noexcept = 0;

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
		virtual sol::table &RawLoad() = 0;

		/**
		 * TODO
		 */
		virtual sol::table &LazyLoad() = 0;

		/**
		 * TODO
		 */
		virtual sol::table &FullLoad() = 0;

		TE_FORCEINLINE const IScriptLoader &GetScriptLoader() const noexcept
		{
			return const_cast<IScriptModule *>(this)->GetScriptLoader();
		}

		TE_FORCEINLINE const sol::table &GetTable() const
		{
			return const_cast<IScriptModule *>(this)->GetTable();
		}
	};

	struct ScriptModule final : public IScriptModule, public IContextProvider, private ILogProvider, public std::enable_shared_from_this<ScriptModule>
	{
	  protected:
		static Context *_parentContext;
		static std::weak_ptr<Log> _parentLog;

	  protected:
		IScriptLoader &_scriptLoader;
		ScriptID _scriptID;
		sol::protected_function _func;
		bool _fullyLoaded;
		bool _hasError;
		sol::table _table;

	  private:
		static void ModuleFieldModifier(std::shared_ptr<ScriptModule> module, sol::object key, sol::object value) noexcept;

	  public:
		explicit ScriptModule(IScriptLoader &scriptLoader) noexcept;
		explicit ScriptModule(IScriptLoader &scriptLoader, ScriptID scriptID, const sol::protected_function &func) noexcept;
		explicit ScriptModule(const ScriptModule &) noexcept = default;
		explicit ScriptModule(ScriptModule &&) noexcept = default;
		ScriptModule &operator=(const ScriptModule &) noexcept = delete;
		ScriptModule &operator=(ScriptModule &&) noexcept = delete;
		~ScriptModule() noexcept = default;

		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		IScriptLoader &GetScriptLoader() noexcept override;
		bool IsValid() const noexcept override;
		bool IsLazyLoaded() const noexcept override;
		bool IsFullyLoaded() const noexcept override;
		bool HasLoadError() const noexcept override;
		void MarkLoadError() noexcept override;

		sol::table &GetTable() override;
		sol::table &RawLoad() override;
		sol::table &LazyLoad() override;
		sol::table &FullLoad() override;
	};
} // namespace tudov