/**
 * @file exception/EventHandlerAddBadKeyException.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "event/EventHandleKey.hpp"
#include "exception/EventHandlerAddException.hpp"
#include "program/Context.hpp"
#include "util/Definitions.hpp"

namespace tudov
{
	class EventHandlerAddBadKeyException : public EventHandlerAddException
	{
	  public:
		EventHandleKey key;

		explicit EventHandlerAddBadKeyException(Context &context, EventID eventID, ScriptID scriptID, EventHandleKey key, std::string traceback) noexcept;
		~EventHandlerAddBadKeyException() noexcept override = default;

		std::string_view What() const noexcept override;
	};
} // namespace tudov
