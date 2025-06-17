#include "LoadtimeEvent.h"

#include "AbstractEvent.h"
#include "OverrideHandlerArgs.hpp"
#include "RuntimeEvent.h"
#include "util/Defs.h"
#include <variant>

using namespace tudov;

LoadtimeEvent::LoadtimeEvent(EventManager &eventManager, EventID eventID, ScriptID scriptID) noexcept
    : AbstractEvent(eventManager, eventID, scriptID),
      _built(false),
      _orders(),
      _keys(),
      _operations()
{
}

LoadtimeEvent::LoadtimeEvent(EventManager &eventManager, EventID eventID, ScriptID scriptID, const std::vector<std::string> &orders, const std::vector<EventHandler::Key> &keys) noexcept
    : AbstractEvent(eventManager, eventID, scriptID),
      _built(true),
      _orders(orders),
      _keys(keys),
      _operations()
{
}

LoadtimeEvent::~LoadtimeEvent() noexcept
{
	//
}

void LoadtimeEvent::Add(const AddHandlerArgs &handler)
{
	_operations.emplace_back(handler);
}

bool LoadtimeEvent::TryBuild(const std::vector<std::string> &orders, const std::vector<EventHandler::Key> &keys) noexcept
{
	if (_built)
	{
		return false;
	}

	_built = true;
	_orders = orders;
	_keys = keys;
	return true;
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
		if (std::holds_alternative<AddHandlerArgs>(operation))
		{
			event.Add(std::get<AddHandlerArgs>(operation));
		}
		else if (std::holds_alternative<OverrideHandlerArgs>(operation))
		{
			// event.Override(Get<EventHandlerOverrider>(operation));
		}
	}

	return std::move(event);
}
