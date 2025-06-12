#include "RuntimeEvent.h"

#include "AbstractEvent.h"
#include "EventHandler.hpp"
#include "mod/ScriptProvider.h"
#include "util/Defs.h"
#include <cassert>

using namespace tudov;

RuntimeEvent::RuntimeEvent(EventID eventID, const Vector<String> &orders, const UnorderedSet<EventHandler::Key, EventHandler::Key::Hash, EventHandler::Key::Equal> &keys, ScriptID scriptID)
    : AbstractEvent(eventID, scriptID),
      _log(Log::Get("RuntimeEvent")),
      _orders(orders),
      _keys(keys)
{
	if (_orders.empty())
	{
		_orders.emplace_back("");
	}
}

void RuntimeEvent::Add(const AddHandlerArgs &args)
{
	auto &&argName = args.name;
	String name;
	if (argName.has_value())
	{
		name = argName.value();
	}
	else
	{
		static UInt64 autoID;
		name = Format("{}-{}", args.scriptID, autoID++);
	}

	auto &&argOrder = args.order;
	auto &&order = argOrder.has_value() ? argOrder.value() : _orders[0];

	auto &&argKey = args.key;
	auto &&key = argKey.has_value() ? argKey.value() : EventHandler::emptyKey;

	auto &&argSequence = args.sequence;
	auto &&sequence = argSequence.has_value() ? argSequence.value() : EventHandler::defaultSequence;

	_handlers.emplace_back(EventHandler{
	    .eventID = args.eventID,
	    .scriptID = args.scriptID,
	    .function = args.function,
	    .name = name,
	    .order = order,
	    .key = key,
	    .sequence = sequence,
	});
	ClearCaches();
}

Vector<EventHandler> &RuntimeEvent::GetSortedHandlers()
{
	if (_handlersSortedCache)
	{
		return _handlers;
	}

	UnorderedMap<String, size_t> orderSequenceMap;
	for (auto &&i = 0; i < _orders.size(); ++i)
	{
		orderSequenceMap[_orders[i]] = i;
	}

	if (!orderSequenceMap.empty())
	{
		Sort(_handlers.begin(), _handlers.end(), [&](const EventHandler &lhs, const EventHandler &rhs)
		{
			auto &&lhsOrder = orderSequenceMap.count(lhs.order) ? orderSequenceMap[lhs.order] : SIZE_MAX;
			auto &&rhsOrder = orderSequenceMap.count(rhs.order) ? orderSequenceMap[rhs.order] : SIZE_MAX;
			if (lhsOrder != rhsOrder)
			{
				return lhsOrder < rhsOrder;
			}
			if (lhs.sequence != rhs.sequence)
			{
				return lhsOrder < rhsOrder;
			}
			return lhs.scriptID < rhs.scriptID;
		});
	}

	_handlersSortedCache = true;

	return _handlers;
}

void RuntimeEvent::Invoke(const sol::object &args, Optional<EventHandler::Key> key)
{
	InvocationCache *cache;
	if (key.has_value())
	{
		auto &&it = _invocationCaches.find(key.value());
		if (it == _invocationCaches.end())
		{
			_invocationCaches[key.value()] = {};
			cache = &_invocationCaches[key.value()];

			for (auto &&handler : GetSortedHandlers())
			{
				if (handler.key == key.value())
				{
					cache->emplace_back(handler.function);
				}
			}
		}
	}
	else
	{
		if (!_invocationCache.has_value())
		{
			_invocationCache = InvocationCache();

			for (auto &&handler : GetSortedHandlers())
			{
				_invocationCache->emplace_back(handler.function);
			}
		}

		cache = &_invocationCache.value();
	}

	if (key.has_value())
	{
		auto &&keyVal = key.value();
		for (auto &&function : *cache)
		{
			function(args, keyVal);
		}
	}
	else
	{
		for (auto &&function : *cache)
		{
			function(args);
		}
	}
}

void RuntimeEvent::InvokeUncached(const sol::object &args, Optional<EventHandler::Key> key)
{
	if (key.has_value())
	{
		auto &&keyVal = key.value();
		for (auto &&handler : _handlers)
		{
			if (handler.key == keyVal)
			{
				handler.function(args, keyVal);
			}
		}
	}
	else
	{
		for (auto &&handler : _handlers)
		{
			handler.function(args);
		}
	}
}

void RuntimeEvent::ClearCaches()
{
	_handlersSortedCache = false;
	_invocationCache = null;
	_invocationCaches.clear();
}

void RuntimeEvent::ClearScriptHandlersImpl(Function<bool(const EventHandler &)> pred)
{
	auto previousSize = _handlers.size();

	EraseIf(_handlers, pred);

	if (_handlers.size() != previousSize)
	{
		ClearCaches();
	}
}

void RuntimeEvent::ClearInvalidScriptsHandlers(const ScriptProvider &scriptProvider)
{
	ClearScriptHandlersImpl([&](const EventHandler &handler)
	{
		return !scriptProvider.IsValidScriptID(handler.scriptID);
	});
}

void RuntimeEvent::ClearScriptsHandlers()
{
	ClearScriptHandlersImpl([](const EventHandler &handler)
	{
		return !!handler.eventID;
	});
	ClearCaches();
}
