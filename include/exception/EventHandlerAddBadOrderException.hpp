/**
 * @file exception/BadEventHandlerOrder.hpp
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
	class EventHandlerAddBadOrderException : public Exception, public IContextProvider
	{
	  public:
		EventID eventID;
		ScriptID scriptID;
		std::string order;
		std::string traceback;

		explicit EventHandlerAddBadOrderException(Context &context, EventID eventID, ScriptID scriptID, std::string order, std::string traceback) noexcept;
		~EventHandlerAddBadOrderException() noexcept override = default;

		Context &GetContext() noexcept override;

		std::string_view What() const noexcept override;
	};
} // namespace tudov
