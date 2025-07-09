#pragma once

#include "EventHandler.hpp"
#include "util/Defs.hpp"

namespace tudov
{
	struct AddHandlerArgs
	{
		using Function = EventHandleFunction;
		using Key = EventHandleKey;

		EventID eventID;
		ScriptID scriptID;
		Function function;
		std::optional<std::string> name;
		std::optional<std::string> order;
		std::optional<Key> key;
		std::optional<std::double_t> sequence;
	};
} // namespace tudov