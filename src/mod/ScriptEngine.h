#pragma once

#include "ScriptLoader.h"
#include "ScriptProvider.h"
#include "program/IEngineComponent.h"
#include "sol/types.hpp"
#include "util/Defs.h"
#include "util/Utils.hpp"

#include <sol/forward.hpp>
#include <sol/load_result.hpp>
#include <sol/sol.hpp>
#include <sol/string_view.hpp>
#include <sol/table.hpp>

#include <memory>
#include <string_view>
#include <unordered_map>

namespace tudov
{
	class Context;

	struct IScriptEngine : IEngineComponent
	{
		virtual size_t GetMemory() const noexcept = 0;
		virtual sol::table CreateTable(std::uint32_t arr = 0, std::uint32_t hash = 0) noexcept = 0;
		virtual void CollectGarbage() = 0;
		virtual sol::object MakeReadonlyGlobal(const sol::object &obj) = 0;
		virtual sol::load_result LoadFunction(const std::string &name, std::string_view code) = 0;
		virtual void InitializeScriptFunction(ScriptID scriptID, std::string_view scriptName, sol::protected_function &function, std::string_view sandboxKey = emptyString) = 0;
		/*
		 * Dangerous function!
		 */
		virtual std::int32_t ThrowError(std::string_view message) noexcept = 0;
	};

	class ScriptEngine : public IScriptEngine
	{
	  private:
		Context _context;
		std::shared_ptr<Log> _log;
		sol::state _lua;
		bool _luaInit;

		std::unordered_map<std::string_view, sol::object> _persistVariables;
		std::unordered_map<std::string_view, sol::table> _sandboxedGlobals;

		sol::function _luaThrowModifyReadonlyGlobalError;
		sol::function _luaInspect;
		sol::function _luaPostProcessSandboxing;
		sol::function _luaMarkAsLocked;

	  public:
		explicit ScriptEngine(Context &context) noexcept;
		~ScriptEngine() noexcept;

	  public:
		Context&GetContext()noexcept override;
		void Initialize() noexcept override;
		void Deinitialize() noexcept override;

		sol::state_view &GetState();

		size_t GetMemory() const noexcept override;
		sol::table CreateTable(std::uint32_t arr = 0, std::uint32_t hash = 0) noexcept override;
		void CollectGarbage() override;
		sol::object MakeReadonlyGlobal(const sol::object &obj) override;
		sol::load_result LoadFunction(const std::string &name, std::string_view code) override;
		void InitializeScriptFunction(ScriptID scriptID, std::string_view scriptName, sol::protected_function &function, std::string_view sandboxKey = emptyString) noexcept override;
		std::int32_t ThrowError(std::string_view message) noexcept override;

		void SetReadonlyGlobal(const sol::string_view &key, sol::object value);
		sol::table &GetSandboxedGlobals(std::string_view sandboxKey) noexcept;
		// void ResetSandboxedGlobal() noexcept;

		sol::object GetPersistVariable(std::string_view key);
		void SetPersistVariable(std::string_view key, const sol::object &value);
	};
} // namespace tudov
