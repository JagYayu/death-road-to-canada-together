/**
 * @file mod/ScriptEngine.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "program/EngineComponent.hpp"
#include "util/Definitions.hpp"
#include "util/Log.hpp"

#include "sol/load_result.hpp"
#include "sol/state.hpp"
#include "util/Utils.hpp"

#include <format>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace tudov
{
	class Context;

	/**
	 * Script Engine.
	 * Basically a wrapper of luaVM.
	 * Also will provide some utilities functions and extensions for *Unique Modding API*.
	 * Mainly used by ModManager.
	 */
	struct IScriptEngine : public IEngineComponent
	{
		virtual ~IScriptEngine() noexcept = default;

		// STD functions.

		virtual void CollectGarbage() = 0;

		virtual sol::table CreateTable(std::uint32_t arr = 0, std::uint32_t hash = 0) noexcept = 0;

		virtual std::string DebugTraceback(std::string_view message = "", std::double_t level = 1) noexcept = 0;

		/**
		 * Get total memory bytes in luaVM.
		 * @return bytes.
		 */
		virtual size_t GetMemory() const noexcept = 0;

		virtual void RawSet(sol::table tbl, sol::object key, sol::object value) = 0;

		[[deprecated("Use `table[::sol::metatable_key] = xxx;` instead")]]
		virtual void SetMetatable(sol::table tbl, sol::metatable mt) = 0;

		// Extended functions.

		virtual sol::object MakeReadonlyGlobal(sol::object obj) = 0;

		virtual sol::load_result LoadFunction(const std::string &name, std::string_view code) = 0;

		virtual void InitializeScript(ScriptID scriptID, std::string_view scriptName, std::string_view modUID, bool sandboxed, sol::protected_function &func) = 0;

		virtual void DeinitializeScript(ScriptID scriptID, std::string_view scriptName) = 0;

		virtual std::string Inspect(sol::object obj) = 0;

		/**
		 * @warning This is a dangerous function (C longjmp), make sure you manually call necessary destructors!
		 */
		[[noreturn]]
		virtual void ThrowError(std::string_view message) noexcept = 0;

		virtual void SetReadonlyGlobal(const std::string_view &key, sol::object value) = 0;

		virtual sol::table &GetModGlobals(std::string_view sandboxKey, bool sandboxed) noexcept = 0;

		virtual sol::object RegisterPersistVariable(std::string_view scriptName, std::string_view key, sol::object defaultValue, const sol::function &getter) = 0;

		virtual std::unordered_map<std::string_view, sol::object> GetScriptPersistVariables(std::string_view scriptName) noexcept = 0;

		virtual void SaveScriptPersistVariables(std::string_view scriptName) noexcept = 0;

		virtual void SavePersistVariables() noexcept = 0;

		virtual bool ClearScriptPersistVariables(std::string_view scriptName) noexcept = 0;

		virtual void ClearPersistVariables() noexcept = 0;

		template <typename... Args>
		[[noreturn]]
		inline void ThrowError(std::format_string<Args...> fmt, Args &&...args) noexcept
		{
			ThrowError(std::format(fmt, std::forward<Args>(args)...));
		}
	};

	class Log;
	class ScriptError;

	class ScriptEngine : public IScriptEngine, private ILogProvider
	{
	  private:
		struct PersistVariable
		{
			sol::object value;
			sol::function getter;

			void Save() noexcept;
		};

	  private:
		Context _context;
		std::shared_ptr<Log> _log;
		sol::state _lua;
		bool _luaInit;

		std::unordered_map<std::string, std::unordered_map<std::string, PersistVariable>> _persistVariables;
		std::unordered_map<std::string_view, sol::table> _modGlobals;

		sol::function _luaThrowModifyReadonlyGlobalError;
		sol::function _luaInspect;
		sol::function _luaPostProcessModGlobals;
		sol::function _luaPostProcessScriptGlobals;
		sol::function _luaMarkAsLocked;

		std::vector<std::shared_ptr<ScriptError>> _scriptRuntimeErrors;

		DelegateEventHandlerID _handlerIDUnloadScript = 0;

	  public:
		explicit ScriptEngine(Context &context) noexcept;
		~ScriptEngine() noexcept = default;

	  public:
		Log &GetLog() noexcept override;
		Context &GetContext() noexcept override;

		void PreInitialize() noexcept override;
		void PostDeinitialize() noexcept override;
		void Initialize() noexcept override;
		void Deinitialize() noexcept override;

		sol::state_view &GetState();

		void CollectGarbage() override;
		sol::table CreateTable(std::uint32_t arr = 0, std::uint32_t hash = 0) noexcept override;
		std::string DebugTraceback(std::string_view message = 0, std::double_t level = 1) noexcept override;
		size_t GetMemory() const noexcept override;
		void RawSet(sol::table tbl, sol::object key, sol::object value) override;
		void SetMetatable(sol::table tbl, sol::metatable mt) override;
		sol::object MakeReadonlyGlobal(sol::object obj) override;
		sol::load_result LoadFunction(const std::string &name, std::string_view code) override;
		void InitializeScript(ScriptID scriptID, std::string_view scriptName, std::string_view modUID, bool sandboxed, sol::protected_function &func) noexcept override;
		void DeinitializeScript(ScriptID scriptID, std::string_view scriptName) override;
		std::string Inspect(sol::object obj) override;
		[[noreturn]]
		void ThrowError(std::string_view message) noexcept override;
		void SetReadonlyGlobal(const std::string_view &key, sol::object value) override;
		sol::table &GetModGlobals(std::string_view sandboxKey, bool sandboxed) noexcept override;
		sol::object RegisterPersistVariable(std::string_view scriptName, std::string_view key, sol::object defaultValue, const sol::function &getter) override;
		std::unordered_map<std::string_view, sol::object> GetScriptPersistVariables(std::string_view scriptName) noexcept override;
		void SaveScriptPersistVariables(std::string_view scriptName) noexcept override;
		void SavePersistVariables() noexcept override;
		bool ClearScriptPersistVariables(std::string_view scriptName) noexcept override;
		void ClearPersistVariables() noexcept override;

	  private:
		void AssertLuaValue(sol::object value, std::string_view name) noexcept;
		sol::object MakeReadonlyGlobalImpl(sol::object obj, std::unordered_map<sol::table, sol::table, LuaTableHash, LuaTableEqual> &visited) noexcept;
	};
} // namespace tudov
