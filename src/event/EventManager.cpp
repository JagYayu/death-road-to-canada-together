/**
 * @file event/EventManager.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "event/EventManager.hpp"

#include "event/AbstractEvent.hpp"
#include "event/CoreEvents.hpp"
#include "event/LoadtimeEvent.hpp"
#include "event/RuntimeEvent.hpp"
#include "exception/EventHandlerAddBadOrderException.hpp"
#include "mod/ModManager.hpp"
#include "mod/ScriptEngine.hpp"
#include "mod/ScriptErrors.hpp"
#include "mod/ScriptLoader.hpp"
#include "program/Context.hpp"
#include "util/Definitions.hpp"
#include "util/LogMicros.hpp"
#include "util/LuaUtils.hpp"
#include "util/Utils.hpp"

#include "sol/types.hpp"
#include <sol/forward.hpp>

#include <cassert>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <variant>

using namespace tudov;

EventManager::EventManager(Context &context) noexcept
    : _context(context),
      _log(Log::Get("EventManager")),
      _latestEventID()
{
	_coreEvents = std::make_unique<CoreEvents>(*this);
}

EventManager::~EventManager() noexcept
{
}

EventID EventManager::AllocEventID(std::string_view eventName) noexcept
{
	_latestEventID++;
	EventID id = _latestEventID;
	auto &&name = _eventID2Name.emplace(id, std::string(eventName)).first->second;
	_eventName2ID.emplace(name, id);
	return id;
}

void EventManager::DeallocEventID(EventID eventID) noexcept
{
	auto &&it = _eventID2Name.find(eventID);
	if (it != _eventID2Name.end())
	{
		_eventName2ID.erase(it->second);
		_eventID2Name.erase(it);
	}
}

void EventManager::OnScriptsLoaded()
{
	auto &&scriptProvider = GetScriptProvider();

	std::vector<std::reference_wrapper<LoadtimeEvent>> invalidEvents;

	// Build loadtime events to runtime events.
	for (auto &&[eventID, loadtimeEvent] : _loadtimeEvents)
	{
		if (!eventID) [[unlikely]]
		{
			throw std::runtime_error("Loadtime events contains event with invalid id!");
		}

		if (_runtimeEvents.contains(eventID))
		{
			auto &&msg = std::format("Loadtime events contains a duplicated event <{}>\"{}\"", eventID, GetEventNameByID(eventID)->data());

			ScriptID scriptID = loadtimeEvent->GetScriptID();
			if (scriptID == 0)
			{
				throw std::runtime_error(msg);
			}

			IScriptProvider &scriptProvider = GetScriptProvider();
			TE_ERROR("{}, source script: <{}>\"{}\"", msg, scriptID, scriptProvider.GetScriptNameByID(scriptID).value_or("$UNKNOWN&"));

			continue;
		}

		if (loadtimeEvent->IsBuilt())
		{
			EmplaceRuntimeEventFromLoadtimeEvent(loadtimeEvent);
		}
		else
		{
			std::string_view eventName = _eventID2Name[eventID];
			ScriptID scriptID = loadtimeEvent->GetScriptID();
			std::string_view scriptName = scriptProvider.GetScriptNameByID(scriptID).value_or("$UNKNOWN$");
			TE_ERROR("Attempt to add handlers to non-exist event <{}>\"{}\", source script <{}>\"{}\"", eventID, eventName, scriptID, scriptName);
		}
	}

	_loadtimeEvents.clear();
	TE_TRACE("{}", "Cleared loadtime events");

	auto &&scriptLoader = GetScriptLoader();

	for (auto &&[_, event] : _runtimeEvents)
	{
		auto scriptID = event->GetScriptID();
		assert(!scriptID || scriptProvider.IsValidScript(scriptID) && "Invalid script detected!");

		if (scriptID)
		{
			for (auto &&it = event->BeginHandlers(); it != event->EndHandlers(); ++it)
			{
				scriptLoader.AddReverseDependency(it->scriptID, scriptID);
			}
		}
	}
}

void EventManager::EmplaceRuntimeEventFromLoadtimeEvent(const std::shared_ptr<LoadtimeEvent> &loadtimeEvent)
{
	std::shared_ptr<RuntimeEvent> runtimeEvent;

	try
	{
		runtimeEvent = std::make_shared<RuntimeEvent>(loadtimeEvent->ToRuntime());

		EventID id = loadtimeEvent->GetID();
		if (id != runtimeEvent->GetID())
		{
			TE_ERROR("Converted runtime event has different id to previous loadtime event! Expected <{}> but got <{}>", id, runtimeEvent->GetID());
		}
		else
		{
			TE_TRACE("Loadtime event converted to a runtime event <{}>\"{}\"", id, GetEventNameByID(id).value_or("$UNKNOWN$"));
			assert(_runtimeEvents.try_emplace(id, runtimeEvent).second);
		}
	}
	catch (const EventHandlerAddBadOrderException &e)
	{
		TE_ERROR("{}", e.What());
		if (e.scriptID != 0)
		{
			GetScriptErrors().AddLoadtimeError(e.scriptID, e.What());
			// The event building failed due to a script error, so we need to add a dependency to the script which creates the event.
			GetScriptLoader().AddReverseDependency(loadtimeEvent->GetScriptID(), e.scriptID);
		}
	}
	catch (std::exception &e)
	{
		UnhandledCppException(_log, e);
	}
}

// ILuaAPI::TInstallation EventManager::rendererLuaAPIInstallation = [](sol::state &lua)
// {
// };

// void EventManager::ProvideLuaAPI(ILuaAPI &luaAPI) noexcept
// {
// 	luaAPI.RegisterInstallation()
// }

Context &EventManager::GetContext() noexcept
{
	return _context;
}

Log &EventManager::GetLog() noexcept
{
	return *_log;
}

ICoreEvents &EventManager::GetCoreEvents() noexcept
{
	return *_coreEvents;
}

void EventManager::PreInitialize() noexcept
{
	auto &&scriptLoader = GetScriptLoader();

	_onPreLoadAllScriptsHandlerID = scriptLoader.GetOnPreLoadAllScripts() += [this]()
	{
		for (auto &&it = _runtimeEvents.begin(); it != _runtimeEvents.end();)
		{
			auto &&runtimeEvent = it->second;
			if (runtimeEvent->GetScriptID())
			{
				DeallocEventID(it->first);
				it = _runtimeEvents.erase(it);
			}
			else
			{
				++it;
			}
		}

		for (auto &&event : _staticEvents)
		{
			event->ClearScriptsHandlers();
		}
	};

	_onPostLoadAllScriptsHandlerID = scriptLoader.GetOnPostLoadAllScripts() += [this]()
	{
		OnScriptsLoaded();
	};

	_onPreHotReloadScriptsHandlerID = scriptLoader.GetOnPreHotReloadScripts() += [this](const std::vector<ScriptID> &scriptIDs)
	{
		if (scriptIDs.empty())
		{
			return;
		}

		ClearInvalidScriptEvents();

		auto &&scriptProvider = GetScriptProvider();
		for (auto &&[_, event] : _runtimeEvents)
		{
			event->ClearInvalidScriptsHandlers(scriptProvider);
		}
	};

	_onPostHotReloadScriptsHandlerID = scriptLoader.GetOnPostHotReloadScripts() += [this](const std::vector<ScriptID> &scriptIDs)
	{
		OnScriptsLoaded();
	};

	_onUnloadScriptHandlerID = scriptLoader.GetOnUnloadScript() += [this](ScriptID scriptID, std::string_view scriptName)
	{
		assert(scriptID && "invalid script id!");

		for (auto &&it = _runtimeEvents.begin(); it != _runtimeEvents.end();)
		{
			if (it->second->GetScriptID() == scriptID)
			{
				EventID eventID = it->second->GetID();
				TE_TRACE("Remove event <{}>\"{}\" from script <{}>\"{}\"", eventID, _eventID2Name[eventID], scriptID, scriptName);

				DeallocEventID(it->first);
				it = _runtimeEvents.erase(it);
			}
			else
			{
				++it;
			}
		}

		auto &&scriptProvider = GetScriptProvider();
		for (auto &&[_, event] : _runtimeEvents)
		{
			event->ClearSpecificScriptHandlers(scriptProvider, scriptID);
		}
	};
}

void EventManager::Initialize() noexcept
{
}

void EventManager::Deinitialize() noexcept
{
}

void EventManager::PostDeinitialize() noexcept
{
	auto &&scriptLoader = GetScriptLoader();

	scriptLoader.GetOnPreLoadAllScripts() -= _onPreLoadAllScriptsHandlerID;
	scriptLoader.GetOnPostLoadAllScripts() -= _onPostLoadAllScriptsHandlerID;
	scriptLoader.GetOnPreHotReloadScripts() -= _onPreHotReloadScriptsHandlerID;
	scriptLoader.GetOnPostHotReloadScripts() -= _onPreHotReloadScriptsHandlerID;
	scriptLoader.GetOnUnloadScript() -= _onUnloadScriptHandlerID;

	_onPreLoadAllScriptsHandlerID = 0;
	_onPostLoadAllScriptsHandlerID = 0;
	_onPreHotReloadScriptsHandlerID = 0;
	_onPreHotReloadScriptsHandlerID = 0;
	_onUnloadScriptHandlerID = 0;
}

void EventManager::InstallToScriptEngine(IScriptEngine &scriptEngine)
{
}

void EventManager::UninstallFromScriptEngine(IScriptEngine &scriptEngine)
{
}

void EventManager::LuaAdd(sol::object event, sol::object func, sol::object name, sol::object order, sol::object key, sol::object sequence)
{
	try
	{
		IScriptEngine &scriptEngine = GetScriptEngine();

		ScriptID scriptID = GetScriptLoader().GetLoadingScriptID();
		if (!scriptID)
		{
			scriptEngine.ThrowError("Failed to add event handler: must add at script load time");
		}

		EventID eventID;
		std::string_view eventName;
		if (event.is<std::double_t>())
		{
			eventID = event.as<std::double_t>();
			auto optEventName = GetEventNameByID(eventID);
			if (!optEventName.has_value()) [[unlikely]]
			{
				scriptEngine.ThrowError("Failed to add event handler: {} is an invalid event id", eventID);
			}
			eventName = optEventName.value();
		}
		else if (event.is<sol::string_view>())
		{
			eventName = event.as<std::string_view>();
			eventID = GetEventIDByName(eventName);
		}
		else
		{
			scriptEngine.ThrowError("Failed to add event handler: invalid arg#1 `event` type, string or number expected, got {}",
			                        GetLuaTypeStringView(event.get_type()));
		}

		if (!eventID)
		{
			eventID = AllocEventID(eventName);
		}

		if (!func.valid() || !func.is<sol::function>())
		{
			scriptEngine.ThrowError(std::format("Failed to add event handler: invalid arg#2 `func` type, expected function, got {}", GetLuaTypeStringView(func.get_type())));
		}
		sol::function func_ = func.as<sol::function>();

		std::optional<std::string> name_;
		if (name.is<sol::nil_t>())
		{
			name_ = std::nullopt;
		}
		else if (name.is<std::string_view>())
		{
			name_ = name.as<std::string_view>();
		}
		else
		{
			LuaUtils::Deconstruct(name_);
			scriptEngine.ThrowError(std::format("Failed to add event handler: invalid arg#3 `name` type, nil or string expected, got {}", GetLuaTypeStringView(name.get_type())));
		}

		std::optional<std::string> order_;
		if (order.is<sol::nil_t>())
		{
			order_ = std::nullopt;
		}
		else if (order.is<std::string_view>())
		{
			order_ = order.as<std::string_view>();
		}
		else
		{
			LuaUtils::Deconstruct(name_, order_);
			scriptEngine.ThrowError(std::format("Failed to add event handler: invalid arg#4 `order` type, nil or string expected, got {}", GetLuaTypeStringView(order.get_type())));
		}

		std::optional<EventHandleKey> key_;
		if (!key.valid() || key.is<sol::nil_t>())
		{
			key_ = std::nullopt;
		}
		else if (key.is<std::double_t>())
		{
			key_ = EventHandleKey(key.as<std::double_t>());
		}
		else if (key.is<std::string_view>())
		{
			key_ = EventHandleKey(std::string(key.as<std::string_view>()));
		}
		else
		{
			LuaUtils::Deconstruct(name_, order_, key_);
			scriptEngine.ThrowError(std::format("Failed to add event handler: invalid arg#5 `key` type, nil or number or number expected, got {}", GetLuaTypeStringView(key.get_type())));
		}

		std::optional<std::double_t> sequence_;
		if (sequence.is<sol::nil_t>())
		{
			sequence_ = std::nullopt;
		}
		else if (sequence.is<std::double_t>())
		{
			sequence_ = sequence.as<std::double_t>();
		}
		else
		{
			LuaUtils::Deconstruct(name_, order_, key_, sequence_);
			scriptEngine.ThrowError(std::format("Failed to add event handler: invalid arg#6 `sequence` type, nil or string expected, got {}", GetLuaTypeStringView(sequence.get_type())));
		}

		auto &&registryEvent = TryGetRegistryEvent(eventID);
		if (!registryEvent.has_value())
		{
			auto &&loadtimeEvent = std::make_shared<LoadtimeEvent>(*this, eventID, scriptID);
			TE_TRACE("Script <{}> added handler to loadtime event <{}>\"{}\"", scriptID, eventID, eventName);
			registryEvent = *(_loadtimeEvents.try_emplace(eventID, loadtimeEvent).first->second);
		}

		registryEvent->get().Add({
		    .eventID = eventID,
		    .scriptID = scriptID,
		    .function = EventHandleFunction(func),
		    .name = name_,
		    .order = order_,
		    .key = key_,
		    .sequence = sequence_,
		    .stacktrace = GetScriptEngine().DebugTraceback(),
		});
	}
	catch (const EventHandlerAddBadOrderException &e)
	{
		TE_ERROR("{}", e.What());
		if (e.scriptID != 0)
		{
			GetScriptErrors().AddLoadtimeError(e.scriptID, e.What());
		}
	}
	catch (const std::exception &e)
	{
		UnhandledCppException(_log, e);
		throw;
	}
}

EventID EventManager::LuaNew(sol::object event, sol::object orders, sol::object keys)
{
	try
	{
		IScriptEngine &scriptEngine = GetScriptEngine();

		ScriptID scriptID = GetScriptLoader().GetLoadingScriptID();
		if (!scriptID)
		{
			scriptEngine.ThrowError("Failed to new event: must call at script load time");
		}

		EventID eventID = false;
		std::string_view eventName = event.as<std::string_view>();
		if (event.is<std::string_view>())
		{
			eventID = GetEventIDByName(eventName);
			if (!eventID)
			{
				eventID = AllocEventID(eventName);
				TE_TRACE("Script <{}> alloc event <{}>\"{}\"", scriptID, eventID, eventName);
			}
		}
		else
		{
			scriptEngine.ThrowError(std::format("Failed to new event: invalid arg#1 `event` type, string expected, got {}", GetLuaTypeStringView(event.get_type())));
		}

		std::vector<std::string> orders_;
		if (orders.is<sol::table>())
		{
			orders_ = {};
			auto &&tbl = orders.as<sol::table>();
			for (std::size_t i = 1;; ++i)
			{
				sol::object val = tbl[i];
				if (val == sol::nil)
				{
					break;
				}
				if (!val.is<std::string_view>())
				{
					LuaUtils::Deconstruct(orders_);
					scriptEngine.ThrowError(std::format("Failed to new event: invalid arg#2 'orders' type, table must be a string array, contains {}", GetLuaTypeStringView(tbl[i].get_type())));
				}
				orders_.emplace_back(val.as<std::string_view>());
			}
		}
		else
		{
			LuaUtils::Deconstruct(orders_);
			scriptEngine.ThrowError(std::format("Failed to new event: invalid arg#2 'orders' type, table expected, got {}", GetLuaTypeStringView(orders.get_type())));
		}

		std::vector<EventHandleKey> keys_;
		if (keys.is<sol::table>())
		{
			keys_ = {};
			auto tbl = keys.as<sol::table>();
			for (std::size_t i = 0; i < tbl.size(); ++i)
			{
				if (tbl[i].is<std::double_t>())
				{
					keys_.emplace_back(tbl[i].get<std::double_t>());
				}
				else if (tbl[i].is<std::string_view>())
				{
					keys_.emplace_back(tbl[i].get<std::string_view>());
				}
				else
				{
					LuaUtils::Deconstruct(orders_, keys_);
					scriptEngine.ThrowError(std::format("Failed to new event: invalid arg#2 'keys' type, table must be a number/string array, contains {}", GetLuaTypeStringView(tbl[i].get_type())));
				}
			}
		}
		else if (keys.is<sol::nil_t>())
		{
			keys_ = {};
		}
		else
		{
			LuaUtils::Deconstruct(orders_, keys_);
			scriptEngine.ThrowError(std::format("Failed to new event: invalid arg#2 'keys' type, table or nil expected, got {}", GetLuaTypeStringView(keys.get_type())));
		}

		std::optional<ScriptID> failed = TryBuildEvent(eventID, scriptID, orders_, keys_);
		if (failed.has_value())
		{
			ScriptID sourceScriptID = failed.has_value();
			std::string_view scriptName = GetScriptProvider().GetScriptNameByID(sourceScriptID).value_or("$UNKNOWN");

			LuaUtils::Deconstruct(orders_, keys_);
			scriptEngine.ThrowError("Failed to new event <{}>\"{}\", already been registered from script <{}>\"{}\"",
			                        eventID, eventName, sourceScriptID, scriptName.data());
		}

		TE_TRACE("Constructed loadtime event <{}>\"{}\" from script <{}>", eventID, eventName, scriptID);
		return eventID;
	}
	catch (const std::exception &e)
	{
		UnhandledCppException(_log, e);
		throw;
	}
}

std::optional<ScriptID> EventManager::TryBuildEvent(EventID eventID, ScriptID scriptID, std::vector<std::string> orders, std::vector<EventHandleKey> keys) noexcept
{
	{
		auto it = _runtimeEvents.find(eventID);
		if (it != _runtimeEvents.end())
		{
			return {it->second->GetScriptID()};
		}
	}

	{
		auto it = _loadtimeEvents.find(eventID);
		if (it == _loadtimeEvents.end())
		{
			auto &&loadtimeEvent = std::make_shared<LoadtimeEvent>(*this, eventID, scriptID, orders, keys);
			_loadtimeEvents.try_emplace(eventID, loadtimeEvent);

			return std::nullopt;
		}

		const std::shared_ptr<LoadtimeEvent> &event = it->second;
		if (!event->TryBuild(scriptID, orders, keys))
		{
			return {event->GetScriptID()};
		}
	}

	return sol::nullopt;
}

void EventManager::LuaInvoke(sol::object event, sol::object args, sol::object key, sol::object options)
{
	try
	{
		IScriptEngine &scriptEngine = GetScriptEngine();

		if (GetScriptLoader().GetLoadingScriptID() != 0) [[unlikely]]
		{
			scriptEngine.ThrowError("Attempt to invoke event at script load time");
		}

		RuntimeEvent *eventInstance;
		if (event.is<EventID>())
		{
			EventID eventID = event.as<EventID>();
			auto it = _runtimeEvents.find(eventID);
			if (it == _runtimeEvents.end())
			{
				LuaUtils::Deconstruct(it);
				scriptEngine.ThrowError(std::format("Runtime event <{}> not found", eventID));
			}
			eventInstance = it->second.get();
		}
		else if (event.is<std::string_view>())
		{
			std::string_view eventName = event.as<std::string_view>();
			auto it = _runtimeEvents.find(_eventName2ID[eventName]);
			if (it == _runtimeEvents.end())
			{
				LuaUtils::Deconstruct(it);
				scriptEngine.ThrowError(std::format("Runtime event <{}> not found", eventName));
			}
			eventInstance = it->second.get();
		}
		else
		{
			scriptEngine.ThrowError("Bad argument #1 to 'event': expected EventID or string");
		}

		EventHandleKey key_;
		if (key.is<double>())
		{
			key_.value = key.as<double>();
		}
		else if (key.is<std::string_view>())
		{
			key_.value = key.as<std::string>();
		}

		EEventInvocation options_;
		if (options.is<std::double_t>())
		{
			options_ = static_cast<EEventInvocation>(options.as<std::double_t>());
		}
		else if (options.is<sol::nil_t>())
		{
			options_ = EEventInvocation::Default;
		}
		else
		{
			LuaUtils::Deconstruct(key_);
			scriptEngine.ThrowError("Bad argument #4 to 'options': expected number or nil");
		}

		assert(eventInstance != nullptr);

		eventInstance->Invoke(args, key_, options_);
	}
	catch (const std::exception &e)
	{
		UnhandledCppException(_log, e);
		throw;
	}
}

EventID EventManager::GetEventIDByName(std::string_view eventName) const noexcept
{
	auto &&it = _eventName2ID.find(std::string(eventName));
	if (it == _eventName2ID.end())
	{
		return false;
	}
	return it->second;
}

std::optional<std::string_view> EventManager::GetEventNameByID(EventID eventID) const noexcept
{
	auto &&it = _eventID2Name.find(eventID);
	if (it == _eventID2Name.end())
	{
		return std::nullopt;
	}
	return it->second;
}

bool EventManager::IsValidEventID(EventID eventID) const noexcept
{
	return _eventID2Name.contains(eventID);
}

void EventManager::ClearInvalidScriptEvents() noexcept
{
	auto &&scriptProvider = GetScriptProvider();

	for (auto &&it = _runtimeEvents.begin(); it != _runtimeEvents.end();)
	{
		auto &&scriptID = it->second->GetScriptID();
		if (scriptID && !scriptProvider.IsValidScript(scriptID))
		{
			DeallocEventID(it->first);
			TE_TRACE("Clear invalid event <{}>, script <{}>", it->first, scriptID);
			it = _runtimeEvents.erase(it);
		}
		else
		{
			++it;
		}
	}
}

std::optional<std::reference_wrapper<AbstractEvent>> EventManager::TryGetRegistryEvent(EventID eventID)
{
	if (!eventID) [[unlikely]]
	{
		throw std::runtime_error("invalid event id");
	}
	{
		auto &&it = _runtimeEvents.find(eventID);
		if (it != _runtimeEvents.end())
		{
			return *(it->second);
		}
	}
	{
		auto &&it = _loadtimeEvents.find(eventID);
		if (it != _loadtimeEvents.end())
		{
			return *(it->second);
		}
	}
	return std::nullopt;
}

std::size_t EventManager::GetRuntimeEventsCount() const noexcept
{
	return _runtimeEvents.size();
}

std::unordered_map<EventID, std::shared_ptr<RuntimeEvent>>::const_iterator EventManager::BeginRuntimeEvents() const
{
	return _runtimeEvents.begin();
}

std::unordered_map<EventID, std::shared_ptr<RuntimeEvent>>::const_iterator EventManager::EndRuntimeEvents() const
{
	return _runtimeEvents.end();
}
