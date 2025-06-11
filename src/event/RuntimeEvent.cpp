#include "RuntimeEvent.h"

#include "AbstractEvent.h"
#include "EventHandler.hpp"
#include "util/Defs.h"
#include "util/StringUtils.hpp"

using namespace tudov;

RuntimeEvent::RuntimeEvent(const String &name)
    : AbstractEvent(emptyString, name),
      _log(Log::Get("RuntimeEvent"))
{
}

RuntimeEvent::RuntimeEvent(const String &scriptName, const String &name)
    : AbstractEvent(scriptName, name),
      _log(Log::Get("RuntimeEvent"))
{
}

void RuntimeEvent::Add(const AddHandlerArgs &args)
{
	auto &&argName = args.name;
	auto &&name = argName.has_value() ? argName.value() : _orders[0];

	auto &&argOrder = args.order;
	auto &&order = argOrder.has_value() ? argOrder.value() : _orders[0];

	auto &&argKey = args.key;
	auto &&key = argKey.has_value() ? argKey.value() : EventHandler::emptyKey;

	auto &&argSequence = args.sequence;
	auto &&sequence = argSequence.has_value() ? argSequence.value() : EventHandler::defaultSequence;

	_handlers.emplace_back(EventHandler{
	    .scriptName = args.scriptName,
	    .event = args.event,
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

	UnorderedMap<String, size_t, StringSVHash, StringSVEqual> orderSequenceMap;
	for (auto &&i = 0; i < _orders.size(); ++i)
	{
		orderSequenceMap[_orders[i]] = i;
	}

	if (!orderSequenceMap.empty())
	{
		Sort(_handlers.begin(), _handlers.end(), [&](const EventHandler &lhs, const EventHandler &rhs)
		{
			size_t lhsOrder = orderSequenceMap.count(lhs.event) ? orderSequenceMap[lhs.event] : SIZE_MAX;
			size_t rhsOrder = orderSequenceMap.count(rhs.event) ? orderSequenceMap[rhs.event] : SIZE_MAX;
			return lhsOrder < rhsOrder;
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

void RuntimeEvent::InvokeUncached(const sol::object &args,
                                  Optional<EventHandler::Key> key)
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
	auto &&previousSize = _handlers.size();

	EraseIf(_handlers, pred);

	if (_handlers.size() != previousSize)
	{
		ClearCaches();
	}
}

void RuntimeEvent::ClearScriptHandlers(const String &scriptName)
{
	if (scriptName == emptyString)
	{
		_log->Warn(Format("Clear script handlers named {} is not permitted", scriptName));
		return;
	}

	ClearScriptHandlersImpl([&](const EventHandler &handler)
	{
		return handler.event == scriptName;
	});
}

void RuntimeEvent::ClearScriptsHandlers()
{
	ClearScriptHandlersImpl([](const EventHandler &handler)
	{
		return handler.event != emptyString;
	});
}
