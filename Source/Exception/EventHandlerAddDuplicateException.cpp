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

#include "exception/EventHandlerAddDuplicateException.hpp"

#include "data/Constants.hpp"
#include "event/EventManager.hpp"
#include "mod/ScriptProvider.hpp"

#include <format>

using namespace tudov;

EventHandlerAddDuplicateException::EventHandlerAddDuplicateException(Context &context, EventID eventID, ScriptID scriptID, std::string name, std::string traceback) noexcept
    : EventHandlerAddException(context, eventID, scriptID, traceback),
      name(name)
{
}

std::string_view EventHandlerAddDuplicateException::What() const noexcept
{
	std::string_view eventName = GetEventManager().GetEventNameByID(eventID).value_or(Constants::ImplStrUnknown);
	std::string_view scriptName = GetScriptProvider().GetScriptNameByID(scriptID).value_or(Constants::ImplStrUnknown);
	_what = std::format("Duplicated event handler name: \"{}\", event <{}>{}, script <{}>{}{}",
	                    name, eventID, eventName, scriptID, scriptName, traceback);
	return _what;
}
