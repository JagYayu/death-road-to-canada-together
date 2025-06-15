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
		SharedPtr<Log> _log;
		sol::state _lua;
		bool _luaInit;

		UnorderedMap<StringView, sol::object> _persistVariables;
		UnorderedMap<StringView, sol::table> _sandboxedGlobals;
		
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

		void SetReadonlyGlobal(const sol::string_view &key, const sol::table &value);
		void CollectGarbage();
		sol::table CreateTable(UInt32 arr = 0, UInt32 hash = 0);
		size_t GetMemory() const noexcept;
		int ThrowError(StringView message);
		sol::load_result LoadFunction(const String &name, StringView code);

		void MarkLockedMetatable();
		sol::object MakeReadonlyGlobal(const sol::object &obj);

		sol::table &GetSandboxedGlobals(StringView sandboxKey) noexcept;
		// void ResetSandboxedGlobal() noexcept;
		void InitializeScriptFunction(ScriptID scriptID, StringView scriptName, sol::protected_function &function, StringView sandboxKey = emptyString) noexcept;

		sol::object GetPersistVariable(StringView key);
		void SetPersistVariable(StringView key, const sol::object &value);
	};
} // namespace tudov
