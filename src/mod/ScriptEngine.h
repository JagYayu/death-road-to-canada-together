#pragma once

#include "sol/forward.hpp"
#include "sol/load_result.hpp"
#include "sol/table.hpp"

#include <sol/sol.hpp>

#include <memory>

namespace sol
{
	class state;
}

namespace tudov
{
	class ModManager;

	class ScriptEngine
	{
	  private:
		ModManager &_modManager;
		sol::state _lua;
		sol::function _traceback;

	  public:
		ScriptEngine(ModManager &gameEngine);

		sol::table CreateTable(uint32_t arr = 0, uint32_t hash = 0);
		sol::load_result LoadFunction(const std::string &name, const std::string &code);
		std::string Traceback(const std::string &what);
		void ReleaseModule(sol::table &module);
		void SetScriptEnv(const std::string &scriptName, const sol::function &func);
	};
} // namespace tudov
