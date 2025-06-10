#pragma once

#include "EventHandler.hpp"
#include "util/Defs.h"

namespace tudov
{
	struct AddHandlerArgs
	{
		using Function = EventHandler::Function;
		using Key = EventHandler::Key;

		String scriptName;
		String name;
		Function function;
		Optional<String> order;
		Optional<Key> key;
		Optional<Number> sequence;
	};
} // namespace tudov