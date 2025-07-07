#pragma once

#include "ScriptEngine.h"

#include "sol/property.hpp"
#include "sol/sol.hpp"

#include <stdexcept>

namespace tudov
{
	class Engine;

	struct ILuaAPI
	{
		virtual void Install(sol::state &lua, Context &context) noexcept = 0;
	};

	class LuaAPI : public ILuaAPI
	{
	  public:
		void Install(sol::state &lua, Context &context) noexcept override;
	};
} // namespace tudov
