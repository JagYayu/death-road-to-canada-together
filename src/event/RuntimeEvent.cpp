#include "RuntimeEvent.h"

#include "EventHandler.hpp"
#include "util/StringUtils.hpp"

using namespace tudov;

void RuntimeEvent::Add(const EventHandler &handler)
{
	_handlers.emplace_back(handler);
}

Vector<EventHandler> &RuntimeEvent::GetSortedHandlers()
{
	if (_handlersSorted)
	{
		return _handlers;
	}

	UnorderedMap<String, size_t, StringTransHash, StringTransEqual> orderSequenceMap;
	for (auto &&i = 0; i < _orders.size(); ++i)
	{
		orderSequenceMap[_orders[i]] = i;
	}

	if (!orderSequenceMap.empty())
	{
		auto compare = [&](const EventHandler &lhs, const EventHandler &rhs)
		{
			size_t lhsOrder = orderSequenceMap.count(lhs.name) ? orderSequenceMap[lhs.name] : SIZE_MAX;
			size_t rhsOrder = orderSequenceMap.count(rhs.name) ? orderSequenceMap[rhs.name] : SIZE_MAX;
			return lhsOrder < rhsOrder;
		};
		Sort(_handlers.begin(), _handlers.end(), compare);
	}

	_handlersSorted = true;

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
			_invocationCache = {};

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
