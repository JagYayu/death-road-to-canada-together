#include "AbstractEvent.h"

#include "util/Defs.h"

using namespace tudov;

AbstractEvent::AbstractEvent(EventID eventID, ScriptID scriptID)
    : _id(eventID),
      _scriptID(scriptID)
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
