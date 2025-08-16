/**
 * @file event/EventHandlerOverrider.hpp
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

namespace tudov
{
	struct EventHandlerOverrider
	{
		using Key = EventHandleKey;

		// TODO
		struct Function
		{
		};

		ScriptID scriptName;
		std::string name;
		Function function;
	};
} // namespace tudov
