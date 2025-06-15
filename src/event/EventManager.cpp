#include "EventManager.h"

#include "AbstractEvent.h"
#include "EventHandler.hpp"
#include "RuntimeEvent.h"
#include "event/LoadtimeEvent.h"
#include "mod/ModManager.h"
#include "mod/ScriptEngine.h"
#include "mod/ScriptLoader.h"
#include "sol/string_view.hpp"
#include "sol/types.hpp"
#include "util/Defs.h"
#include "util/Utils.hpp"

#include <sol/forward.hpp>
#include <string_view>

using namespace tudov;

EventManager::EventManager(ModManager &modManager)
    : modManager(modManager),
      _log(Log::Get("EventManager")),
      _latestEventID(),
      update(std::make_shared<RuntimeEvent>(*this, AllocEventID("Update"))),
      render(std::make_shared<RuntimeEvent>(*this, AllocEventID("Render")))
{
	_runtimeEvents.emplace(update->GetID(), update);
	_runtimeEvents.emplace(render->GetID(), render);
}

EventManager::~EventManager()
{
}

EventID EventManager::AllocEventID(std::string_view eventName) noexcept
{
	{
		auto &&it = _eventName2ID.find(eventName);
		if (it != _eventName2ID.end())
		{
			_log->Warn("Event name already exists: {}", eventName);
			return it->second;
		}
	}

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
	std::vector<Reference<LoadtimeEvent>> invalidEvents;

	for (auto &&[eventID, loadtimeEvent] : _loadtimeEvents)
	{
		auto &&it = _runtimeEvents.find(eventID);
		if (it != _runtimeEvents.end())
		{
			_log->Error("Duplicated registering event named \"{}\": '{}' -> '{}'",
			            GetEventNameByID(eventID).value(),
			            modManager.scriptProvider.GetScriptNameByID(loadtimeEvent->GetScriptID()).value(),
			            modManager.scriptProvider.GetScriptNameByID(it->second->GetScriptID()).value());
		}
		else
		{
			auto &&runtimeEvent = std::make_shared<RuntimeEvent>(loadtimeEvent->ToRuntime());
			_runtimeEvents.emplace(runtimeEvent->GetID(), runtimeEvent);
		}
	}

	_loadtimeEvents = {};
}

void EventManager::RegisterGlobal(ScriptEngine &scriptEngine)
{
	auto &&table = scriptEngine.CreateTable(0);

	auto &&scriptLoader = scriptEngine.scriptLoader;

	table["add"] = [&](const sol::object &event, const sol::object &func, const sol::object &name, const sol::object &order, const sol::object &key, const sol::object &sequence)
	{
		try
		{
			auto loadingScript = scriptLoader.GetLoadingScriptID();
			if (!loadingScript)
			{
				scriptEngine.ThrowError("Failed to add event handler: must add at script load time");
				return;
			}

			if (!event.valid() || !event.is<sol::string_view>())
			{
				scriptEngine.ThrowError(Format("Failed to add event handler: invalid arg#1 `event` type, expected string, got {}", GetLuaTypeStringView(event.get_type())));
				return;
			}
			std::string_view event_ = event.as<sol::string_view>();

			auto &&eventID = GetEventIDByName(event_);
			auto &&eventInstance = TryGetRegistryEvent(eventID);
			if (!eventInstance.has_value())
			{
				scriptEngine.ThrowError(Format("Failed to add event handler: event named '{}' not found", event_));
				return;
			}

			if (!func.valid() || !func.is<sol::function>())
			{
				modManager.scriptEngine.ThrowError(Format("Failed to add event handler: invalid arg#2 `func` type, expected function, got {}", GetLuaTypeStringView(func.get_type())));
				return;
			}
			sol::function func_ = func.as<sol::function>();

			std::optional<std::string> name_;
			if (name.is<sol::nil_t>())
			{
				name_ = nullopt;
			}
			else if (name.is<sol::string_view>())
			{
				name_ = name.as<sol::string_view>();
			}
			else
			{
				modManager.scriptEngine.ThrowError(Format("Failed to add event handler: invalid arg#3 `name` type, nil or string expected, got {}", GetLuaTypeStringView(name.get_type())));
				return;
			}

			std::optional<std::string> order_;
			if (order.is<sol::nil_t>())
			{
				order_ = nullopt;
			}
			else if (order.is<sol::string_view>())
			{
				order_ = order.as<sol::string_view>();
			}
			else
			{
				modManager.scriptEngine.ThrowError(Format("Failed to add event handler: invalid arg#4 `order` type, nil or string expected, got {}", GetLuaTypeStringView(order.get_type())));
				return;
			}

			std::optional<AddHandlerArgs::Key> key_;
			if (!key.valid() || key.is<sol::nil_t>())
			{
				key_ = nullopt;
			}
			else if (key.is<Number>())
			{
				key_ = AddHandlerArgs::Key(key.as<Number>());
			}
			else if (key.is<sol::string_view>())
			{
				key_ = AddHandlerArgs::Key(std::string(key.as<sol::string_view>()));
			}
			else
			{
				modManager.scriptEngine.ThrowError(Format("Failed to add event handler: invalid arg#5 `key` type, nil or number or number expected, got {}", GetLuaTypeStringView(key.get_type())));
				return;
			}

			std::optional<Number> sequence_;
			if (sequence.is<sol::nil_t>())
			{
				sequence_ = nullopt;
			}
			else if (sequence.is<Number>())
			{
				sequence_ = sequence.as<Number>();
			}
			else
			{
				modManager.scriptEngine.ThrowError(Format("Failed to add event handler: invalid arg#6 `sequence` type, nil or string expected, got {}", GetLuaTypeStringView(sequence.get_type())));
				return;
			}

			eventInstance->get().Add({
			    .eventID = eventID,
			    .scriptID = loadingScript,
			    .function = EventHandler::Function(func),
			    .name = name_,
			    .order = order_,
			    .key = key_,
			    .sequence = sequence_,
			});
		}
		catch (const std::exception &e)
		{
			UnhandledCppException(_log, e);
			throw;
		}
	};

	table["new"] = [&](const sol::object &event, const sol::object &orders, const sol::object &keys)
	{
		try
		{
			auto &&scriptID = scriptLoader.GetLoadingScriptID();
			if (!scriptID)
			{
				scriptEngine.ThrowError("Failed to new event: must call at script load time");
				return;
			}

			EventID eventID = false;
			if (event.is<sol::string_view>())
			{
				eventID = GetEventIDByName(event.as<sol::string_view>());
				if (eventID)
				{
					scriptEngine.ThrowError(Format("Failed to new event: invalid arg#1 `event` type, event already exists"));
					return;
				}
			}
			else
			{
				scriptEngine.ThrowError(Format("Failed to new event: invalid arg#1 `event` type, string expected, got {}", GetLuaTypeStringView(event.get_type())));
				return;
			}

			std::vector<std::string> orders_;
			if (orders.is<sol::table>())
			{
				orders_ = {};
				auto &&tbl = orders.as<sol::table>();
				for (size_t i = 0; i < tbl.size(); ++i)
				{
					if (!tbl[i].is<sol::string_view>())
					{
						scriptEngine.ThrowError(Format("Failed to new event: invalid arg#2 'orders' type, table must be a string array, contains {}", GetLuaTypeStringView(tbl[i].get_type())));
						return;
					}
					orders_.emplace_back(tbl[i].get<sol::string_view>());
				}
			}
			else
			{
				scriptEngine.ThrowError(Format("Failed to new event: invalid arg#2 'orders' type, table expected, got {}", GetLuaTypeStringView(orders.get_type())));
				return;
			}

			std::vector<EventHandler::Key> keys_;
			if (keys.is<sol::table>())
			{
				keys_ = {};
				auto &&tbl = keys.as<sol::table>();
				for (size_t i = 0; i < tbl.size(); ++i)
				{
					if (!tbl[i].is<Number>() && !tbl[i].is<sol::string_view>())
					{
						scriptEngine.ThrowError(Format("Failed to new event: invalid arg#2 'keys' type, table must be a number/string array, contains {}", GetLuaTypeStringView(tbl[i].get_type())));
						return;
					}
					keys_.emplace_back(EventHandler::Key{
					    .value = tbl[i],
					});
				}
			}
			else
			{
				scriptEngine.ThrowError(Format("Failed to new event: invalid arg#2 'keys' type, table expected, got {}", GetLuaTypeStringView(keys.get_type())));
				return;
			}

			_loadtimeEvents.try_emplace(eventID, std::make_shared<LoadtimeEvent>(*this, eventID, orders_, keys_, scriptID));
		}
		catch (const std::exception &e)
		{
			UnhandledCppException(_log, e);
			throw;
		}
	};

	scriptEngine.SetReadonlyGlobal("Events", table);

	_onPreLoadAllScriptsHandlerID = scriptLoader.onPreLoadAllScripts += [&]()
	{
		for (auto it = _runtimeEvents.begin(); it != _runtimeEvents.end();)
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

	_onPostLoadAllScriptsHandlerID = scriptLoader.onPostLoadAllScripts += [&]()
	{
		OnScriptsLoaded();
	};

	_onPreHotReloadScriptsHandlerID = scriptLoader.onPreHotReloadScripts += [this](const std::vector<ScriptID> &scriptIDs)
	{
		if (scriptIDs.empty())
		{
			return;
		}

		for (auto it = _runtimeEvents.begin(); it != _runtimeEvents.end();)
		{
			for (auto scriptID : scriptIDs)
			{
				auto &&event = it->second;
				if (event->GetScriptID() == scriptID)
				{
					DeallocEventID(it->first);
					it = _runtimeEvents.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		for (auto &&[_, event] : _runtimeEvents)
		{
			event->ClearInvalidScriptsHandlers(modManager.scriptProvider);
		}
	};

	_onPreHotReloadScriptsHandlerID = scriptLoader.onPostHotReloadScripts += [&](const std::vector<ScriptID> &scriptIDs)
	{
		OnScriptsLoaded();
	};
}

void EventManager::UnregisterGlobal(ScriptEngine &scriptEngine)
{
	scriptEngine.scriptLoader.onPreLoadAllScripts -= _onPreLoadAllScriptsHandlerID;
	scriptEngine.scriptLoader.onPostLoadAllScripts -= _onPostLoadAllScriptsHandlerID;
	scriptEngine.scriptLoader.onPreHotReloadScripts -= _onPreHotReloadScriptsHandlerID;
	scriptEngine.scriptLoader.onPostHotReloadScripts -= _onPreHotReloadScriptsHandlerID;
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
		return nullopt;
	}
	return it->second;
}

bool EventManager::IsValidEventID(EventID eventID) const noexcept
{
	return _eventID2Name.contains(eventID);
}

std::optional<Reference<AbstractEvent>> EventManager::TryGetRegistryEvent(EventID eventID)
{
	if (!eventID)
	{
		return nullopt;
	}
	{
		auto &&it = _loadtimeEvents.find(eventID);
		if (it != _loadtimeEvents.end())
		{
			return *(it->second);
		}
	}
	{
		auto &&it = _runtimeEvents.find(eventID);
		if (it != _runtimeEvents.end())
		{
			return *(it->second);
		}
	}
	return nullopt;
}

std::unordered_map<EventID, SharedPtr<RuntimeEvent>>::const_iterator EventManager::BeginRuntimeEvents() const
{
	return _runtimeEvents.begin();
}

std::unordered_map<EventID, SharedPtr<RuntimeEvent>>::const_iterator EventManager::EndRuntimeEvents() const
{
	return _runtimeEvents.end();
}
