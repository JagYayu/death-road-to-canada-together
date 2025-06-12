#pragma once

#include "EventHandler.hpp"
#include "util/Defs.h"

namespace tudov
{
	struct AddHandlerArgs
	{
		using Function = EventHandler::Function;
		using Key = EventHandler::Key;

		EventID eventID;
		ScriptID scriptID;
		Function function;
		Optional<String> name;
		Optional<String> order;
		Optional<Key> key;
		Optional<Number> sequence;
	};
} // namespace tudov