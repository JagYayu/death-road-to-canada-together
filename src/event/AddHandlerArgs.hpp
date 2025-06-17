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
		std::optional<std::string> name;
		std::optional<std::string> order;
		std::optional<Key> key;
		std::optional<std::double_t> sequence;
	};
} // namespace tudov