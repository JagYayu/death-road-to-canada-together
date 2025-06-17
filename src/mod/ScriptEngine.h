#pragma once

#include "ScriptLoader.h"
#include "ScriptProvider.h"
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
	class ModManager;

	class ScriptEngine
	{
	  private:
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
		ModManager &modManager;
		ScriptLoader scriptLoader;

	  public:
		ScriptEngine(ModManager &modManager);
		~ScriptEngine();

	  public:
		void Initialize();

		sol::state_view &GetState();

		void SetReadonlyGlobal(const sol::string_view &key, sol::object value);
		void CollectGarbage();
		sol::table CreateTable(std::uint32_t arr = 0, std::uint32_t hash = 0);
		size_t GetMemory() const noexcept;
		sol::object MakeReadonlyGlobal(const sol::object &obj);
		void MarkLockedMetatable();
		int ThrowError(std::string_view message);
		sol::load_result LoadFunction(const std::string &name, std::string_view code);

		sol::table &GetSandboxedGlobals(std::string_view sandboxKey) noexcept;
		// void ResetSandboxedGlobal() noexcept;
		void InitializeScriptFunction(ScriptID scriptID, std::string_view scriptName, sol::protected_function &function, std::string_view sandboxKey = emptyString) noexcept;

		sol::object GetPersistVariable(std::string_view key);
		void SetPersistVariable(std::string_view key, const sol::object &value);
	};
} // namespace tudov
