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

#include "exception/EventHandlerAddBadOrderException.hpp"
#include "event/EventManager.hpp"
#include "mod/ScriptProvider.hpp"

#include <format>

using namespace tudov;

EventHandlerAddBadOrderException::EventHandlerAddBadOrderException(Context &context, EventID eventID, ScriptID scriptID, std::string order, std::string traceback) noexcept
    : eventID(eventID),
      scriptID(scriptID),
      order(order),
      traceback(traceback),
      Exception(context)
{
}

Context &EventHandlerAddBadOrderException::GetContext() noexcept
{
	return _context;
}

std::string_view EventHandlerAddBadOrderException::What() const noexcept
{
	std::string_view eventName = GetEventManager().GetEventNameByID(eventID).value_or("$UNKNOWN$");
	std::string_view scriptName = GetScriptProvider().GetScriptNameByID(eventID).value_or("$UNKNOWN$");
	_what = std::format("Bad event handler order: \"{}\", event <{}>\"{}\", script <{}>\"{}\"{}",
	                    order, eventID, eventName, scriptID, scriptName, traceback);
	return _what;
}
