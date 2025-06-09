#pragma once

#include "RuntimeEvent.h"
#include "util/Defs.h"

#include "sol/sol.hpp"
#include <string_view>

namespace tudov
{
	class GameEngine;

	class EventManager
	{
	  private:
		GameEngine &_gameEngine;

	  public:
		RuntimeEvent update;
		RuntimeEvent render;

		explicit EventManager(GameEngine &gameEngine);

		void Initialize(sol::state &lua);

		void ClearScriptHandlers();
		void ClearScriptHandlers(std::string_view);
		
		void HandleRuntimeTransition();
	};
} // namespace tudov
