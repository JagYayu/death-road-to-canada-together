/**
 * @file Exception/EventHandlerAddException.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "exception/EventHandlerException.hpp"
#include "Program/Context.hpp"
#include "Util/Definitions.hpp"

namespace tudov
{
	class EventHandlerAddException : public ScriptException
	{
	  public:
		EventID eventID;

		explicit EventHandlerAddException(Context &context, EventID eventID, ScriptID scriptID, std::string traceback) noexcept;
		explicit EventHandlerAddException(const EventHandlerAddException &) noexcept = default;
		explicit EventHandlerAddException(EventHandlerAddException &&) noexcept = default;
		EventHandlerAddException &operator=(const EventHandlerAddException &) noexcept = delete;
		EventHandlerAddException &operator=(EventHandlerAddException &&) noexcept = delete;
		~EventHandlerAddException() noexcept override = default;

		std::string_view What() const noexcept override;
	};
} // namespace tudov
