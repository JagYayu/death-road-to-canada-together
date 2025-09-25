/**
 * @file exception/EventHandlerException.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "exception/ScriptException.hpp"
#include "program/Context.hpp"
#include "util/Definitions.hpp"

namespace tudov
{
	class EventHandlerException : public ScriptException
	{
	  public:
		EventID eventID;

		explicit EventHandlerException(Context &context, EventID eventID, ScriptID scriptID, std::string traceback) noexcept;
				explicit EventHandlerException(const EventHandlerException &) noexcept = default;
		explicit EventHandlerException(EventHandlerException &&) noexcept = default;
		EventHandlerException &operator=(const EventHandlerException &) noexcept = delete;
		EventHandlerException &operator=(EventHandlerException &&) noexcept = delete;
		~EventHandlerException() noexcept override = default;

		std::string_view What() const noexcept override;
	};
} // namespace tudov
