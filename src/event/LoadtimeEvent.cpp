/**
 * @file event/LoadtimeEvent.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "event/LoadtimeEvent.hpp"

#include "event/AbstractEvent.hpp"
#include "event/OverrideHandlerArgs.hpp"
#include "event/RuntimeEvent.hpp"
#include "util/Definitions.hpp"
#include "util/LogMicros.hpp"

#include <variant>

using namespace tudov;

LoadtimeEvent::LoadtimeEvent(IEventManager &eventManager, EventID eventID, ScriptID scriptID) noexcept
    : AbstractEvent(eventManager, eventID, scriptID),
      _built(false),
      _orders(),
      _keys(),
      _operations()
{
}

LoadtimeEvent::LoadtimeEvent(IEventManager &eventManager, EventID eventID, ScriptID scriptID, const std::vector<std::string> &orders, const std::vector<EventHandleKey> &keys) noexcept
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

bool LoadtimeEvent::IsBuilt() const noexcept
{
	return _built;
}

bool LoadtimeEvent::TryBuild(ScriptID scriptID, const std::vector<std::string> &orders, const std::vector<EventHandleKey> &keys) noexcept
{
	if (_built)
	{
		return false;
	}

	_built = true;
	_scriptID = scriptID;
	_orders = orders;
	_keys = keys;
	return true;
}

RuntimeEvent LoadtimeEvent::ToRuntime() const
{
	std::unordered_set<EventHandleKey, EventHandleKey::Hash, EventHandleKey::Equal> keys{};
	for (const EventHandleKey &key : _keys)
	{
		keys.emplace(key);
	}

	RuntimeEvent event{
	    eventManager,
	    _id,
	    _orders,
	    keys,
	    _scriptID,
	};

	for (const Operation &operation : _operations)
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

	return event;
}
