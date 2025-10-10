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

#include "EventHandleKey.hpp"
#include "Event/EventHandleFunction.hpp"
#include "Util/Definitions.hpp"

namespace tudov
{
	struct AddHandlerArgs
	{
		EventHandleFunction function;
		std::optional<std::string> name;
		std::optional<std::string> order;
		std::optional<EventHandleKey> key;
		std::optional<std::double_t> sequence;
		ScriptID scriptID;
		std::string stacktrace;
	};
} // namespace tudov