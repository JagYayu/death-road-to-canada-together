/**
 * @file exception/EventHandlerAddException.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "exception/Exception.hpp"
#include "program/Context.hpp"
#include "util/Definitions.hpp"

namespace tudov
{
	class EventHandlerAddException : public Exception
	{
	  public:
		EventID eventID;
		ScriptID scriptID;
		std::string traceback;

		explicit EventHandlerAddException(Context &context, EventID eventID, ScriptID scriptID, std::string traceback) noexcept;
		~EventHandlerAddException() noexcept override = default;

		std::string_view What() const noexcept override;
	};
} // namespace tudov
