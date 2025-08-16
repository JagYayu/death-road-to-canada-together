/**
 * @file event/EventHandler.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "EventHandleFunction.hpp"
#include "EventHandleKey.hpp"
#include "util/Definitions.hpp"

#include <sol/sol.hpp>

#include <cmath>

namespace tudov
{
	struct EventHandler
	{
		inline static constexpr std::double_t defaultSequence = 0;
		inline static const EventHandleKey emptyKey = EventHandleKey();

		EventID eventID;
		ScriptID scriptID;
		EventHandleFunction function;
		std::string name;
		std::string order;
		EventHandleKey key;
		std::double_t sequence;
	};
} // namespace tudov