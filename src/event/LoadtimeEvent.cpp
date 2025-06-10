#include "LoadtimeEvent.h"

#include "EventHandlerOverrider.hpp"
#include "OverrideHandlerArgs.hpp"
#include "RuntimeEvent.h"
#include "util/Defs.h"

using namespace tudov;

void LoadtimeEvent::Add(const AddHandlerArgs &handler)
{
	operations.emplace_back(handler);
}

RuntimeEvent LoadtimeEvent::ToRuntime()
{
	RuntimeEvent event{_scriptName, _name};

	for (auto &&operation : operations)
	{
		if (Is<AddHandlerArgs>(operation))
		{
			event.Add(Get<AddHandlerArgs>(operation));
		}
		else if (Is<OverrideHandlerArgs>(operation))
		{
			// event.Override(Get<EventHandlerOverrider>(operation));
		}
	}

	return Move(event);
}
