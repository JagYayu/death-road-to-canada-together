#pragma once

#include "EventHandler.hpp"

namespace tudov
{
	struct EventHandlerOverrider
	{
		using Key = EventHandler::Key;

		// TODO
		struct Function
		{
		};

		ScriptID scriptName;
		String name;
		Function function;
	};
} // namespace tudov
