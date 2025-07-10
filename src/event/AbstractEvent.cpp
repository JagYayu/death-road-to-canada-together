#include "AbstractEvent.hpp"

#include "util/Defs.hpp"

using namespace tudov;

AbstractEvent::AbstractEvent(IEventManager &eventManager, EventID eventID, ScriptID scriptID)
    : eventManager(eventManager),
      _id(eventID),
      _scriptID(scriptID)
{
}

AbstractEvent::~AbstractEvent() noexcept
{
}

EventID AbstractEvent::GetID() const noexcept
{
	return _id;
}

ScriptID AbstractEvent::GetScriptID() const noexcept
{
	return _scriptID;
}
