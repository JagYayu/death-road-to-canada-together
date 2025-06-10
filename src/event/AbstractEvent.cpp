#include "AbstractEvent.h"

#include "util/Defs.h"

using namespace tudov;

AbstractEvent::AbstractEvent(const String &scriptName, const String &name)
    : _scriptName(scriptName),
      _name(name)
{
}
const String &AbstractEvent::GetName() const noexcept
{
	return _name;
}

const String &AbstractEvent::GetScriptName() const noexcept
{
	return _scriptName;
}
