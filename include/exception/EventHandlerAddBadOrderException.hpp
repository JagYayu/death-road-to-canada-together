/**
 * @file exception/EventHandlerAddBadOrderException.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "EventHandlerAddException.hpp"
#include "Program/Context.hpp"
#include "Util/Definitions.hpp"

namespace tudov
{
	class EventHandlerAddBadOrderException : public EventHandlerAddException
	{
	  public:
		std::string order;

		explicit EventHandlerAddBadOrderException(Context &context, EventID eventID, ScriptID scriptID, std::string order, std::string traceback) noexcept;
		explicit EventHandlerAddBadOrderException(const EventHandlerAddBadOrderException &) noexcept = default;
		explicit EventHandlerAddBadOrderException(EventHandlerAddBadOrderException &&) noexcept = default;
		EventHandlerAddBadOrderException &operator=(const EventHandlerAddBadOrderException &) noexcept = delete;
		EventHandlerAddBadOrderException &operator=(EventHandlerAddBadOrderException &&) noexcept = delete;
		~EventHandlerAddBadOrderException() noexcept override = default;

		std::string_view What() const noexcept override;
	};
} // namespace tudov
