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

#include "Exception/EventHandlerAddException.hpp"

#include "Data/Constants.hpp"
#include "Event/EventManager.hpp"
#include "mod/ScriptProvider.hpp"

#include <format>

using namespace tudov;

EventHandlerAddException::EventHandlerAddException(Context &context, EventID eventID, ScriptID scriptID, std::string traceback) noexcept
    : ScriptException(context, scriptID, traceback),
      eventID(eventID)
{
}

std::string_view EventHandlerAddException::What() const noexcept
{
	std::string_view eventName = GetEventManager().GetEventNameByID(eventID).value_or(Constants::ImplStrUnknown);
	std::string_view scriptName = GetScriptProvider().GetScriptNameByID(scriptID).value_or(Constants::ImplStrUnknown);
	_what = std::format("Bad event handler event <{}>{}, script <{}>{}{}",
	                    eventID, eventName, scriptID, scriptName, traceback);
	return _what;
}
