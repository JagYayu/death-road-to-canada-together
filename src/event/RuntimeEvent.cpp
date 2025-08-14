#include "event/RuntimeEvent.hpp"
#include "event/AbstractEvent.hpp"
#include "event/CoreEventsData.hpp"
#include "event/EventHandler.hpp"
#include "event/EventManager.hpp"
#include "event/RuntimeEvent.hpp"
#include "mod/ScriptProvider.hpp"
#include "util/Definitions.hpp"
#include "util/EnumFlag.hpp"

#include <cassert>
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
			return lhs.scriptID < rhs.scriptID;
		});
	}

	_handlersSortedCache = true;

	return _handlers;
}

template <typename... TArgs>
TE_FORCEINLINE void PCallHandler(std::shared_ptr<Log> &log, EventHandler &handler, TArgs &&...args)
{
	try
	{
		handler.function(std::forward<TArgs>(args)...);
	}
	catch (std::exception &e)
	{
		log->Error("{}", e.what());
	}
}

void RuntimeEvent::Invoke(const sol::object &e, const EventHandleKey &key, EInvocation options)
{
	Profile *profile;
	if (_profile != nullptr && !EnumFlag::HasAny(options, EInvocation::NoProfiler))
	{
		profile = _profile.get();
	}
	else
	{
		profile = nullptr;
	}

	TInvocationTrackID trackID;
	if (EnumFlag::HasAny(options, EInvocation::TrackProgression))
	{
		trackID = _invocationTrackID;
		++_invocationTrackID;
	}

	IScriptEngine &scriptEngine = eventManager.GetScriptEngine();

	bool anyKey = key.IsAny();

	if (profile)
	{
		profile->eventProfiler.BeginEvent(scriptEngine);
	}

	if (EnumFlag::HasAny(options, EInvocation::CacheHandlers))
	{
		TInvocationCache *cache;

		if (anyKey)
		{
			if (!_invocationCache.has_value()) [[unlikely]]
			{
				_invocationCache = TInvocationCache();

				for (auto &&handler : GetSortedHandlers())
				{
					_invocationCache->emplace_back(&handler);
				}
			}

			cache = &_invocationCache.value();
		}
		else if (auto &&it = _invocationCaches.find(key); it != _invocationCaches.end()) [[likely]]
		{
			cache = &it->second;
		}
		else [[unlikely]]
		{
			cache = &_invocationCaches.try_emplace(key, TInvocationCache()).first->second;
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
			for (EventHandler *handler : *cache)
			{
				PCallHandler(_log, *handler, e);
			}
		}
		else
		{
			for (EventHandler *handler : *cache)
			{
				PCallHandler(_log, *handler, e, key);
			}
		}
	}
	else // * !EnumFlag::HasAny(options, EInvocation::CacheHandlers)
	{
		if (anyKey)
		{
			if (_profile && _profile->traceHandlers)
			{
				for (EventHandler &handler : _handlers)
				{
					PCallHandler(_log, handler, e);
					_profile->eventProfiler.TraceHandler(scriptEngine, handler.name);
				}
			}
			else
			{
				for (EventHandler &handler : _handlers)
				{
					PCallHandler(_log, handler, e, key);
				}
			}
		}
		else
		{
			if (_profile && _profile->traceHandlers)
			{
				for (EventHandler &handler : _handlers)
				{
					if (handler.key.Match(key))
					{
						PCallHandler(_log, handler, e);
						_profile->eventProfiler.TraceHandler(scriptEngine, handler.name);
					}
				}
			}
			else
			{
				for (EventHandler &handler : _handlers)
				{
					if (handler.key.Match(key))
					{
						PCallHandler(_log, handler, e, key);
					}
				}
			}
		}
	}

	if (profile)
	{
		profile->eventProfiler.EndEvent(scriptEngine);
	}
}

void RuntimeEvent::Invoke(IScriptEngine &scriptEngine, CoreEventData *data, const EventHandleKey &key, EInvocation options)
{
	if (data == nullptr) [[unlikely]]
	{
		abort();
	}

	sol::table &&args = scriptEngine.CreateTable(0, 1);
	args["data"] = data;
	Invoke(args, key, options);
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
	_log->Trace("Runtime event <{}> cleared caches", _id);
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
