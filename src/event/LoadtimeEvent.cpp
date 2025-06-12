#include "LoadtimeEvent.h"

#include "OverrideHandlerArgs.hpp"
#include "RuntimeEvent.h"
#include "util/Defs.h"

using namespace tudov;

void LoadtimeEvent::Add(const AddHandlerArgs &handler)
{
	_operations.emplace_back(handler);
}

RuntimeEvent LoadtimeEvent::ToRuntime()
{
	RuntimeEvent event{_id, _orders, _keys, _scriptID};

	for (auto &&operation : _operations)
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
