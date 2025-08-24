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

#include "exception/EventHandlerAddException.hpp"

#include "event/EventManager.hpp"
#include "mod/ScriptProvider.hpp"

#include <format>

using namespace tudov;

EventHandlerAddException::EventHandlerAddException(Context &context, EventID eventID, ScriptID scriptID, std::string traceback) noexcept
    : Exception(context),
      eventID(eventID),
      scriptID(scriptID),
      traceback(traceback)
{
}

std::string_view EventHandlerAddException::What() const noexcept
{
	std::string_view eventName = GetEventManager().GetEventNameByID(eventID).value_or("$UNKNOWN$");
	std::string_view scriptName = GetScriptProvider().GetScriptNameByID(eventID).value_or("$UNKNOWN$");
	_what = std::format("Bad event handler event <{}>\"{}\", script <{}>\"{}\"{}",
	                    eventID, eventName, scriptID, scriptName, traceback);
	return _what;
}
