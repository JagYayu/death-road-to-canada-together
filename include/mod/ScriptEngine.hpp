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

		/**
		 * Get total memory bytes in luaVM.
		 * @return bytes.
		 */
		virtual size_t GetMemory() const noexcept = 0;

		virtual sol::table CreateTable(std::uint32_t arr = 0, std::uint32_t hash = 0) noexcept = 0;

		virtual void CollectGarbage() = 0;

		virtual sol::object MakeReadonlyGlobal(sol::object obj) = 0;

		virtual sol::load_result LoadFunction(const std::string &name, std::string_view code) = 0;

		virtual void InitializeScript(ScriptID scriptID, std::string_view scriptName, std::string_view modUID, bool sandboxed, sol::protected_function &func) = 0;

		virtual void DeinitializeScript(ScriptID scriptID, std::string_view scriptName) = 0;

		virtual std::string Inspect(sol::object obj) = 0;

		/*
		 * @warning This is a dangerous function.
		 */
		virtual std::int32_t ThrowError(std::string_view message) noexcept = 0;

		/*
		 * @warning This is a dangerous function.
		 */
		template <typename... Args>
		inline std::int32_t ThrowError(std::format_string<Args...> fmt, Args &&...args) noexcept
		{
			return ThrowError(std::format(fmt, std::forward<Args>(args)...));
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

		size_t GetMemory() const noexcept override;
		sol::table CreateTable(std::uint32_t arr = 0, std::uint32_t hash = 0) noexcept override;
		void CollectGarbage() override;
		sol::object MakeReadonlyGlobal(sol::object obj) override;
		sol::load_result LoadFunction(const std::string &name, std::string_view code) override;
		void InitializeScript(ScriptID scriptID, std::string_view scriptName, std::string_view modUID, bool sandboxed, sol::protected_function &func) noexcept override;
		void DeinitializeScript(ScriptID scriptID, std::string_view scriptName) override;
		std::string Inspect(sol::object obj) override;
		std::int32_t ThrowError(std::string_view message) noexcept override;

		void SetReadonlyGlobal(const std::string_view &key, sol::object value);
		sol::table &GetModGlobals(std::string_view sandboxKey, bool sandboxed) noexcept;
		// void ResetSandboxedGlobal() noexcept;

		sol::object RegisterPersistVariable(std::string_view scriptName, std::string_view key, sol::object defaultValue, const sol::function &getter);
		void ClearPersistVariables();

	  private:
		void AssertLuaValue(sol::object value, std::string_view name) noexcept;
		sol::object MakeReadonlyGlobalImpl(sol::object obj, std::unordered_map<sol::table, sol::table, LuaTableHash, LuaTableEqual> &visited) noexcept;
	};
} // namespace tudov
