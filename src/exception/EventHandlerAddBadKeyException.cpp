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

#include "exception/EventHandlerAddBadKeyException.hpp"

#include "event/EventManager.hpp"
#include "mod/ScriptProvider.hpp"

#include <format>

using namespace tudov;

EventHandlerAddBadKeyException::EventHandlerAddBadKeyException(Context &context, EventID eventID, ScriptID scriptID, EventHandleKey key, std::string traceback) noexcept
    : EventHandlerAddException(context, eventID, scriptID, traceback),
      key(key)
{
}

std::string_view EventHandlerAddBadKeyException::What() const noexcept
{
	std::string_view eventName = GetEventManager().GetEventNameByID(eventID).value_or("$UNKNOWN$");
	std::string_view scriptName = GetScriptProvider().GetScriptNameByID(eventID).value_or("$UNKNOWN$");
	_what = std::format("Bad event handler key: \"{}\", event <{}>\"{}\", script <{}>\"{}\"{}",
	                    key.ToString(), eventID, eventName, scriptID, scriptName, traceback);
	return _what;
}
