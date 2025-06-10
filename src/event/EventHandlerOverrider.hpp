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

		String scriptName;
		String name;
		Function function;
	};
} // namespace tudov
