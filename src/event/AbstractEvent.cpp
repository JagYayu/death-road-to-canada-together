/**
 * @file event/AbstractEvent.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "event/AbstractEvent.hpp"
#include "event/EventManager.hpp"
#include "util/Definitions.hpp"

using namespace tudov;

AbstractEvent::AbstractEvent(IEventManager &eventManager, EventID eventID, ScriptID scriptID)
    : eventManager(eventManager),
      _eventID(eventID),
      _scriptID(scriptID)
{
}

AbstractEvent::~AbstractEvent() noexcept
{
}

Context &AbstractEvent::GetContext() noexcept
{
	return eventManager.GetContext();
}

EventID AbstractEvent::GetID() const noexcept
{
	return _eventID;
}

ScriptID AbstractEvent::GetScriptID() const noexcept
{
	return _scriptID;
}
