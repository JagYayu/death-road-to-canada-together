#include "RuntimeEvent.hpp"

#include "AbstractEvent.hpp"
#include "EventHandler.hpp"
#include "EventManager.hpp"
#include "event/RuntimeEvent.hpp"
#include "mod/ModManager.hpp"
#include "mod/ScriptProvider.hpp"
#include "util/Defs.hpp"

#include "sol/types.hpp"

#include <cassert>
#include <cstddef>
#include <variant>

using namespace tudov;

RuntimeEvent::RuntimeEvent(IEventManager &eventManager, EventID eventID, const std::vector<std::string> &orders, const std::unordered_set<EventHandleKey, EventHandleKey::Hash, EventHandleKey::Equal> &keys, ScriptID scriptID)
    : AbstractEvent(eventManager, eventID, scriptID),
      _log(Log::Get("RuntimeEvent")),
      _orders(orders),
      _keys(keys)
{
	if (_orders.empty())
	{
		_orders.emplace_back("");
	}
}

RuntimeEvent::~RuntimeEvent() noexcept
{
}

std::vector<EventHandler>::const_iterator RuntimeEvent::BeginHandlers() const noexcept
{
	return _handlers.begin();
}

std::vector<EventHandler>::const_iterator RuntimeEvent::EndHandlers() const noexcept
{
	return _handlers.end();
}

std::optional<std::reference_wrapper<RuntimeEvent::Profile>> RuntimeEvent::GetProfile() const noexcept
{
	if (_profile)
	{
		return *_profile;
	}
	return std::nullopt;
}

void RuntimeEvent::EnableProfiler(bool traceHandlers) noexcept
{
	if (!_profile)
	{
		_profile = std::make_unique<Profile>(traceHandlers);
	}
	else
	{
		_profile->traceHandlers = traceHandlers;
	}
}

void RuntimeEvent::DisableProfiler() noexcept
{
	_profile = nullptr;
}

void RuntimeEvent::Add(const AddHandlerArgs &args)
{
	auto &&argName = args.name;
	std::string name;
	if (argName.has_value())
	{
		name = argName.value();
	}
	else
	{
		static std::uint64_t autoID;
		name = std::format("{}-{}", args.scriptID, autoID++);
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

std::vector<EventHandler> &RuntimeEvent::GetSortedHandlers()
{
	if (_handlersSortedCache)
	{
		return _handlers;
	}

	std::unordered_map<std::string, size_t> orderSequenceMap;
	for (auto &&i = 0; i < _orders.size(); ++i)
	{
		orderSequenceMap[_orders[i]] = i;
	}

	if (!orderSequenceMap.empty())
	{
		std::sort(_handlers.begin(), _handlers.end(), [&](const EventHandler &lhs, const EventHandler &rhs)
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

void RuntimeEvent::Invoke(const sol::object &args, EventHandleKey key)
{
	if (_profile && _profile->traceHandlers)
	{
		InvokeUncached(args, key);
		return;
	}

	InvocationCache *cache;
	if (!std::holds_alternative<std::nullptr_t>(key.value))
	{
		auto &&it = _invocationCaches.find(key);
		if (it != _invocationCaches.end())
		{
			cache = &it->second;
		}
		else
		{
			cache = &_invocationCaches.try_emplace(key, InvocationCache()).first->second;
			for (auto &&handler : GetSortedHandlers())
			{
				if (handler.key == key)
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

	if (_profile)
	{
		_profile->eventProfiler.BeginEvent(*eventManager.GetScriptEngine());
	}

	try
	{
		if (!std::holds_alternative<std::nullptr_t>(key.value))
		{
			for (auto &&function : *cache)
			{
				function(args, key);
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
	catch (std::exception &e)
	{
		_log->Error("{}", e.what());
	}

	if (_profile)
	{
		_profile->eventProfiler.EndEvent(*eventManager.GetScriptEngine());
	}
}

void RuntimeEvent::InvokeUncached(const sol::object &args, EventHandleKey key)
{
	if (_profile)
	{
		_profile->eventProfiler.BeginEvent(*eventManager.GetScriptEngine());
	}

	try
	{
		if (!std::holds_alternative<std::nullptr_t>(key.value))
		{
			for (auto &&handler : _handlers)
			{
				if (handler.key == key)
				{
					handler.function(args, key);

					if (_profile)
					{
						_profile->eventProfiler.TraceHandler(*eventManager.GetScriptEngine(), handler.name);
					}
				}
			}
		}
		else
		{
			for (auto &&handler : _handlers)
			{
				handler.function(args);

				if (_profile)
				{
					_profile->eventProfiler.TraceHandler(*eventManager.GetScriptEngine(), handler.name);
				}
			}
		}
	}
	catch (std::exception &e)
	{
		_log->Error("{}", e.what());
	}

	if (_profile)
	{
		_profile->eventProfiler.EndEvent(*eventManager.GetScriptEngine());
	}
}

void RuntimeEvent::ClearCaches()
{
	_handlersSortedCache = false;
	_invocationCache = std::nullopt;
	_invocationCaches.clear();
}

void RuntimeEvent::ClearScriptHandlersImpl(std::function<bool(const EventHandler &)> pred)
{
	auto previousSize = _handlers.size();

	std::erase_if(_handlers, pred);

	if (_handlers.size() != previousSize)
	{
		ClearCaches();
	}
}

void RuntimeEvent::ClearInvalidScriptsHandlers(const IScriptProvider &scriptProvider)
{
	ClearScriptHandlersImpl([&](const EventHandler &handler)
	{
		return !scriptProvider.IsValidScript(handler.scriptID);
	});
}

void RuntimeEvent::ClearSpecificScriptHandlers(const IScriptProvider &scriptProvider, ScriptID scriptID)
{
	ClearScriptHandlersImpl([this, scriptID](const EventHandler &handler)
	{
		return handler.scriptID == scriptID;
	});
}

void RuntimeEvent::ClearScriptsHandlers()
{
	ClearScriptHandlersImpl([](const EventHandler &handler)
	{
		return !!handler.eventID;
	});
}
