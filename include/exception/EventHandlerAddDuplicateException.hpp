/**
 * @file exception/EventHandlerAddDuplicateException.hpp
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
#include "program/Context.hpp"
#include "util/Definitions.hpp"

namespace tudov
{
	class EventHandlerAddDuplicateException : public EventHandlerAddException
	{
	  public:
		std::string name;

		explicit EventHandlerAddDuplicateException(Context &context, EventID eventID, ScriptID scriptID, std::string name, std::string traceback) noexcept;
		explicit EventHandlerAddDuplicateException(const EventHandlerAddDuplicateException &) noexcept = default;
		explicit EventHandlerAddDuplicateException(EventHandlerAddDuplicateException &&) noexcept = default;
		EventHandlerAddDuplicateException &operator=(const EventHandlerAddDuplicateException &) noexcept = delete;
		EventHandlerAddDuplicateException &operator=(EventHandlerAddDuplicateException &&) noexcept = delete;
		~EventHandlerAddDuplicateException() noexcept override = default;

		std::string_view What() const noexcept override;
	};
} // namespace tudov
