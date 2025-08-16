/**
 * @file event/AddHandlerArgs.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "EventHandler.hpp"
#include "util/Definitions.hpp"

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