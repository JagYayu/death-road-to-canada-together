/**
 * @file event/RuntimeEvent.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "event/RuntimeEvent.hpp"

#include "debug/EventProfiler.hpp"
#include "event/AbstractEvent.hpp"
#include "event/CoreEventsData.hpp"
#include "event/EventHandler.hpp"
#include "event/EventManager.hpp"
#include "event/RuntimeEvent.hpp"
#include "exception/EventHandlerAddBadOrderException.hpp"
#include "mod/ScriptEngine.hpp"
#include "mod/ScriptErrors.hpp"
#include "mod/ScriptProvider.hpp"
#include "system/LogMicros.hpp"
#include "util/Definitions.hpp"
#include "util/EnumFlag.hpp"

#include <algorithm>

#include <cstddef>
#include <memory>
#include <utility>

using namespace tudov;

RuntimeEvent::RuntimeEvent(IEventManager &eventManager, EventID eventID, const std::vector<std::string> &orders, const std::unordered_set<EventHandleKey, EventHandleKey::Hash, EventHandleKey::Equal> &keys, ScriptID scriptID)
    : AbstractEvent(eventManager, eventID, scriptID),
      _log(Log::Get("RuntimeEvent")),
      _orders(orders),
      _keys(keys),
      _invocationTrackID(0)
{
	if (_orders.empty())
	{
		_orders.emplace_back("");
	}
}

RuntimeEvent::~RuntimeEvent() noexcept
{
}

Log &RuntimeEvent::GetLog() noexcept
{
	return *_log;
}

std::vector<EventHandler>::const_iterator RuntimeEvent::BeginHandlers() const noexcept
{
	return _handlers.begin();
}

std::vector<EventHandler>::const_iterator RuntimeEvent::EndHandlers() const noexcept
{
	return _handlers.end();
}

RuntimeEvent::Profile *RuntimeEvent::GetProfile() const noexcept
{
	if (_profile)
	{
		return _profile.get();
	}
	return nullptr;
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
	auto optArgName = args.name;
	std::string name;
	if (optArgName.has_value())
	{
		name = optArgName.value();
	}
	else
	{
		static std::uint64_t autoID;
		name = std::format("{}-{}", args.scriptID, autoID++);
	}

	auto optArgOrder = args.order;
	std::string order = optArgOrder.has_value() ? optArgOrder.value() : _orders[0];

	auto it = std::find(_orders.begin(), _orders.end(), order);
	if (it == _orders.end()) [[unlikely]]
	{
		// auto &&message = std::format("Invalid event handler order \"{}\"", order);
		throw EventHandlerAddBadOrderException(GetContext(), args.eventID, args.scriptID, order, args.stacktrace);
	}

	auto optArgKey = args.key;
	const EventHandleKey &key = optArgKey.has_value() ? optArgKey.value() : EventHandler::emptyKey;

	auto optArgSequence = args.sequence;
	std::double_t sequence = optArgSequence.has_value() ? optArgSequence.value() : EventHandler::defaultSequence;

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
		std::sort(_handlers.begin(), _handlers.end(), [this, &orderSequenceMap](const EventHandler &lhs, const EventHandler &rhs)
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
			return lhs.name < rhs.name;
		});
	}

	_handlersSortedCache = true;

	return _handlers;
}

struct PCallHandlerObject
{
	const std::shared_ptr<Log> &log;
	IEventManager &eventManager;
};

template <typename... TArgs>
TE_FORCEINLINE void PCallHandler(const PCallHandlerObject &obj, EventHandler &handler, TArgs &&...args) noexcept
{
	try
	{
		handler.function(std::forward<TArgs>(args)...);
	}
	catch (std::exception &e)
	{
		obj.log->Error("{}", e.what());

		if (handler.scriptID != 0)
		{
			obj.eventManager.GetScriptErrors().AddRuntimeError(handler.scriptID, e.what());
		}
	}
}

void RuntimeEvent::Invoke(const sol::object &e, const EventHandleKey &key, EEventInvocation options)
{
	IScriptEngine &scriptEngine = eventManager.GetScriptEngine();

	Profile *profile;
	if (_profile != nullptr && !EnumFlag::HasAny(options, EEventInvocation::NoProfiler))
	{
		profile = _profile.get();
		profile->eventProfiler->BeginEvent(scriptEngine);
	}
	else
	{
		profile = nullptr;
	}

	PCallHandlerObject obj{
	    _log,
	    eventManager,
	};

	bool anyKey = key.IsAny();

	Progression *progression;
	if (EnumFlag::HasAny(options, EEventInvocation::TrackProgression))
	{
		++_invocationTrackID;
		_invocationTracks[_invocationTrackID] = {0, 0};
	}
	else
	{
		progression = nullptr;
	}

	if (EnumFlag::HasAny(options, EEventInvocation::CacheHandlers))
	{
		InvocationCache *cache;

		if (anyKey)
		{
			if (!_invocationCache.has_value()) [[unlikely]]
			{
				_invocationCache = InvocationCache();

				for (auto &&handler : GetSortedHandlers())
				{
					_invocationCache->emplace_back(&handler);
				}
			}

			cache = &_invocationCache.value();
		}
		else if (auto it = _invocationCaches.find(key); it != _invocationCaches.end()) [[likely]]
		{
			cache = &it->second;
		}
		else [[unlikely]]
		{
			cache = &_invocationCaches.try_emplace(key, InvocationCache()).first->second;
			for (auto &&handler : GetSortedHandlers())
			{
				if (handler.key.Match(key))
				{
					cache->emplace_back(&handler);
				}
			}
		}

		if (anyKey)
		{
			if (_profile && _profile->traceHandlers)
			{
				if (progression != nullptr)
				{
					progression->total = cache->size();
					for (EventHandler *handler : *cache)
					{
						PCallHandler(obj, *handler, e);
						_profile->eventProfiler->TraceHandler(scriptEngine, handler->name);
						++progression->value;
					}
				}
				else
				{
					for (EventHandler *handler : *cache)
					{
						PCallHandler(obj, *handler, e);
						_profile->eventProfiler->TraceHandler(scriptEngine, handler->name);
					}
				}
			}
			else
			{
				if (progression != nullptr)
				{
					progression->total = cache->size();
					for (EventHandler *handler : *cache)
					{
						PCallHandler(obj, *handler, e);
						++progression->value;
					}
				}
				else
				{
					for (EventHandler *handler : *cache)
					{
						PCallHandler(obj, *handler, e);
					}
				}
			}
		}
		else
		{
			if (progression != nullptr)
			{
				progression->total = cache->size();
				for (EventHandler *handler : *cache)
				{
					PCallHandler(obj, *handler, e, key);
					++progression->value;
				}
			}
			else
			{
				for (EventHandler *handler : *cache)
				{
					PCallHandler(obj, *handler, e, key);
				}
			}
		}
	}
	else // * !EnumFlag::HasAny(options, EEventInvocation::CacheHandlers)
	{
		if (anyKey)
		{
			if (_profile && _profile->traceHandlers)
			{
				if (progression != nullptr)
				{
					progression->total = _handlers.size();
					for (EventHandler &handler : _handlers)
					{
						PCallHandler(obj, handler, e);
						_profile->eventProfiler->TraceHandler(scriptEngine, handler.name);
						++progression->value;
					}
				}
				else
				{
					for (EventHandler &handler : _handlers)
					{
						PCallHandler(obj, handler, e);
						_profile->eventProfiler->TraceHandler(scriptEngine, handler.name);
					}
				}
			}
			else
			{
				if (progression != nullptr)
				{
					progression->total = _handlers.size();
					for (EventHandler &handler : _handlers)
					{
						PCallHandler(obj, handler, e, key);
						++progression->value;
					}
				}
				else
				{
					for (EventHandler &handler : _handlers)
					{
						PCallHandler(obj, handler, e, key);
					}
				}
			}
		}
		else
		{
			if (_profile && _profile->traceHandlers)
			{
				if (progression != nullptr)
				{
					progression->total = _handlers.size();
					for (EventHandler &handler : _handlers)
					{
						if (handler.key.Match(key))
						{
							PCallHandler(obj, handler, e);
							_profile->eventProfiler->TraceHandler(scriptEngine, handler.name);
							++progression->value;
						}
					}
				}
				else
				{
					for (EventHandler &handler : _handlers)
					{
						if (handler.key.Match(key))
						{
							PCallHandler(obj, handler, e);
							_profile->eventProfiler->TraceHandler(scriptEngine, handler.name);
						}
					}
				}
			}
			else
			{
				if (progression != nullptr)
				{
					progression->total = _handlers.size();
					for (EventHandler &handler : _handlers)
					{
						if (handler.key.Match(key))
						{
							PCallHandler(obj, handler, e, key);
							++progression->value;
						}
					}
				}
				else
				{
					for (EventHandler &handler : _handlers)
					{
						if (handler.key.Match(key))
						{
							PCallHandler(obj, handler, e, key);
						}
					}
				}
			}
		}
	}

	if (profile)
	{
		profile->eventProfiler->EndEvent(scriptEngine);
	}
}

void RuntimeEvent::Invoke(CoreEventData *data, const EventHandleKey &key, EEventInvocation options)
{
	if (data != nullptr)
	{
		sol::table args = GetScriptEngine().CreateTable(0, 1);
		args["data"] = data;
		Invoke(args, key, options);
	}
	else
	{
		Invoke(GetScriptEngine().CreateTable(), key, options);
	}
}

void RuntimeEvent::InvokeUncached(const sol::object &args, const EventHandleKey &key)
{
	Invoke(args, key);
}

void RuntimeEvent::ClearCaches()
{
	_handlersSortedCache = false;
	_invocationCache = std::nullopt;
	_invocationCaches.clear();
	TE_TRACE("Runtime event <{}> cleared caches", _id);
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
	ClearScriptHandlersImpl([this, &scriptProvider](const EventHandler &handler)
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
