#include "LoadtimeEvent.h"

#include "AbstractEvent.h"
#include "OverrideHandlerArgs.hpp"
#include "RuntimeEvent.h"
#include "util/Defs.h"

using namespace tudov;

LoadtimeEvent::LoadtimeEvent(EventManager &eventManager, EventID eventID, const std::vector<std::string> &orders, const std::vector<EventHandler::Key> &keys, ScriptID scriptID) noexcept
    : AbstractEvent(eventManager, eventID, scriptID),
      _orders(orders),
      _keys(keys)
{
}

void LoadtimeEvent::Add(const AddHandlerArgs &handler)
{
	_operations.emplace_back(handler);
}

RuntimeEvent LoadtimeEvent::ToRuntime() noexcept
{
	std::unordered_set<EventHandler::Key, EventHandler::Key::Hash, EventHandler::Key::Equal> keys{};
	for (auto &&key : _keys)
	{
		keys.emplace(key);
	}

	_orders.shrink_to_fit();
	RuntimeEvent event{eventManager, _id, _orders, keys, _scriptID};

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
