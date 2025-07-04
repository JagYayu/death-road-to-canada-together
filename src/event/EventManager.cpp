#include "EventManager.h"

#include "AbstractEvent.h"
#include "RuntimeEvent.h"
#include "event/LoadtimeEvent.h"
#include "mod/ModManager.h"
#include "mod/ScriptEngine.h"
#include "mod/ScriptLoader.h"
#include "util/Defs.h"
#include "util/Utils.hpp"

#include "sol/string_view.hpp"
#include "sol/types.hpp"
#include <sol/forward.hpp>

#include <cassert>
#include <memory>
#include <optional>
#include <string_view>

using namespace tudov;

EventManager::EventManager(ModManager &modManager)
    : modManager(modManager),
      _log(Log::Get("EventManager")),
      _latestEventID(),
      update(std::make_shared<RuntimeEvent>(*this, AllocEventID("Update"))),
      render(std::make_shared<RuntimeEvent>(*this, AllocEventID("Render"))),
      keyDown(std::make_shared<RuntimeEvent>(*this, AllocEventID("KeyDown"))),
      keyUp(std::make_shared<RuntimeEvent>(*this, AllocEventID("KeyUp"))),
      mouseMove(std::make_shared<RuntimeEvent>(*this, AllocEventID("MouseMove"))),
      mouseButtonDown(std::make_shared<RuntimeEvent>(*this, AllocEventID("MouseButtonDown"))),
      mouseButtonUp(std::make_shared<RuntimeEvent>(*this, AllocEventID("MouseButtonUp"))),
      mouseWheel(std::make_shared<RuntimeEvent>(*this, AllocEventID("MouseWheel")))
{
	_runtimeEvents.emplace(update->GetID(), update);
	_runtimeEvents.emplace(render->GetID(), render);
}

EventManager::~EventManager()
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
	std::vector<std::reference_wrapper<LoadtimeEvent>> invalidEvents;

	for (auto &&[eventID, loadtimeEvent] : _loadtimeEvents)
	{
		assert(eventID && "contains invalid event id!");
		assert(_runtimeEvents.find(eventID) == _runtimeEvents.end() && "duplicated event id!");

		if (loadtimeEvent->IsBuilt())
		{
			auto &&runtimeEvent = std::make_shared<RuntimeEvent>(loadtimeEvent->ToRuntime());
			_runtimeEvents.try_emplace(runtimeEvent->GetID(), runtimeEvent);
		}
		else
		{
			auto &&eventName = _eventID2Name[eventID];
			auto scriptID = loadtimeEvent->GetScriptID();
			auto scriptName = modManager.scriptProvider.GetScriptNameByID(scriptID).value_or("$UNKNOWN$");
			_log->Error("Attempt to add handler to non-exist event <{}>\"{}\", source script <{}>\"{}\"", eventID, eventName, scriptID, scriptName);
		}
	}

	_loadtimeEvents.clear();

	auto &&scriptLoader = modManager.scriptEngine.scriptLoader;

	for (auto &&[_, event] : _runtimeEvents)
	{
		auto scriptID = event->GetScriptID();
		if (scriptID)
		{
			for (auto &&it = event->BeginHandlers(); it != event->EndHandlers(); ++it)
			{
				scriptLoader.AddReverseDependency(it->scriptID, scriptID);
			}
		}
	}
}

void EventManager::InstallToScriptEngine(ScriptEngine &scriptEngine)
{
	auto &&table = scriptEngine.CreateTable();

	auto &&scriptLoader = scriptEngine.scriptLoader;

	table.set_function("add", [&](const sol::object &event, const sol::object &func, const sol::object &name, const sol::object &order, const sol::object &key, const sol::object &sequence)
	{
		try
		{
			auto scriptID = scriptLoader.GetLoadingScriptID();
			if (!scriptID)
			{
				scriptEngine.ThrowError("Failed to add event handler: must add at script load time");
				return;
			}

			if (!event.valid() || !event.is<sol::string_view>())
			{
				scriptEngine.ThrowError(std::format("Failed to add event handler: invalid arg#1 `event` type, expected string, got {}", GetLuaTypeStringView(event.get_type())));
				return;
			}
			std::string_view eventName = event.as<sol::string_view>();

			if (!func.valid() || !func.is<sol::function>())
			{
				modManager.scriptEngine.ThrowError(std::format("Failed to add event handler: invalid arg#2 `func` type, expected function, got {}", GetLuaTypeStringView(func.get_type())));
				return;
			}
			sol::function func_ = func.as<sol::function>();

			std::optional<std::string> name_;
			if (name.is<sol::nil_t>())
			{
				name_ = std::nullopt;
			}
			else if (name.is<sol::string_view>())
			{
				name_ = name.as<sol::string_view>();
			}
			else
			{
				modManager.scriptEngine.ThrowError(std::format("Failed to add event handler: invalid arg#3 `name` type, nil or string expected, got {}", GetLuaTypeStringView(name.get_type())));
				return;
			}

			std::optional<std::string> order_;
			if (order.is<sol::nil_t>())
			{
				order_ = std::nullopt;
			}
			else if (order.is<sol::string_view>())
			{
				order_ = order.as<sol::string_view>();
			}
			else
			{
				modManager.scriptEngine.ThrowError(std::format("Failed to add event handler: invalid arg#4 `order` type, nil or string expected, got {}", GetLuaTypeStringView(order.get_type())));
				return;
			}

			std::optional<AddHandlerArgs::Key> key_;
			if (!key.valid() || key.is<sol::nil_t>())
			{
				key_ = std::nullopt;
			}
			else if (key.is<std::double_t>())
			{
				key_ = AddHandlerArgs::Key(key.as<std::double_t>());
			}
			else if (key.is<sol::string_view>())
			{
				key_ = AddHandlerArgs::Key(std::string(key.as<sol::string_view>()));
			}
			else
			{
				modManager.scriptEngine.ThrowError(std::format("Failed to add event handler: invalid arg#5 `key` type, nil or number or number expected, got {}", GetLuaTypeStringView(key.get_type())));
				return;
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
				modManager.scriptEngine.ThrowError(std::format("Failed to add event handler: invalid arg#6 `sequence` type, nil or string expected, got {}", GetLuaTypeStringView(sequence.get_type())));
				return;
			}

			auto &&eventID = GetEventIDByName(eventName);
			if (!eventID)
			{
				eventID = AllocEventID(eventName);
			}
			auto &&registryEvent = TryGetRegistryEvent(eventID);
			if (!registryEvent.has_value())
			{
				auto &&e = std::make_shared<LoadtimeEvent>(*this, eventID, scriptID);
				_log->Trace("Add script <{}> event \"{}\"", scriptID, eventName);
				registryEvent = *(_loadtimeEvents.try_emplace(eventID, e).first->second);
			}

			registryEvent->get().Add({
			    .eventID = eventID,
			    .scriptID = scriptID,
			    .function = EventHandleFunction(func),
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
	});

	table.set_function("new", [&](const sol::object &event, const sol::object &orders, const sol::object &keys) -> EventID
	{
		try
		{
			auto &&scriptID = scriptLoader.GetLoadingScriptID();
			if (!scriptID)
			{
				scriptEngine.ThrowError("Failed to new event: must call at script load time");
				return false;
			}

			EventID eventID = false;
			std::string_view eventName = event.as<sol::string_view>();
			if (event.is<sol::string_view>())
			{
				eventID = GetEventIDByName(eventName);
				if (!eventID)
				{
					eventID = AllocEventID(eventName);
					_log->Trace("Add script <{}> event \"{}\"", scriptID, eventName);
				}
			}
			else
			{
				scriptEngine.ThrowError(std::format("Failed to new event: invalid arg#1 `event` type, string expected, got {}", GetLuaTypeStringView(event.get_type())));
				return false;
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
					if (!val.is<sol::string_view>())
					{
						scriptEngine.ThrowError(std::format("Failed to new event: invalid arg#2 'orders' type, table must be a string array, contains {}", GetLuaTypeStringView(tbl[i].get_type())));
						return false;
					}
					orders_.emplace_back(val.as<sol::string_view>());
				}
			}
			else
			{
				scriptEngine.ThrowError(std::format("Failed to new event: invalid arg#2 'orders' type, table expected, got {}", GetLuaTypeStringView(orders.get_type())));
				return false;
			}

			std::vector<EventHandleKey> keys_;
			if (keys.is<sol::table>())
			{
				keys_ = {};
				auto &&tbl = keys.as<sol::table>();
				for (size_t i = 0; i < tbl.size(); ++i)
				{
					if (!tbl[i].is<std::double_t>() && !tbl[i].is<sol::string_view>())
					{
						scriptEngine.ThrowError(std::format("Failed to new event: invalid arg#2 'keys' type, table must be a number/string array, contains {}", GetLuaTypeStringView(tbl[i].get_type())));
						return false;
					}
					keys_.emplace_back(EventHandleKey{tbl[i]});
				}
			}
			else if (keys.is<sol::nil_t>())
			{
				keys_ = {};
			}
			else
			{
				scriptEngine.ThrowError(std::format("Failed to new event: invalid arg#2 'keys' type, table or nil expected, got {}", GetLuaTypeStringView(keys.get_type())));
				return false;
			}

			bool dup = false;
			{
				auto &&it = _loadtimeEvents.find(eventID);
				if (it != _loadtimeEvents.end() && !it->second->TryBuild(orders_, keys_))
				{
					dup = true;
				}
			}
			if (dup)
			{
				scriptEngine.ThrowError(std::format("Failed to new event <{}>\"{}\", already been registered", eventID, eventName));
				return false;
			}

			auto &&loadtimeEvent = std::make_shared<LoadtimeEvent>(*this, eventID, scriptID, orders_, keys_);
			return _loadtimeEvents.try_emplace(eventID, loadtimeEvent).first->first;
		}
		catch (const std::exception &e)
		{
			UnhandledCppException(_log, e);
			throw;
		}
	});

	table.set_function("invoke", [&](const sol::object &event, const sol::object &args, const sol::object &key) -> void
	{
		try
		{
			RuntimeEvent *eventInstance;
			if (event.is<EventID>())
			{
				auto &&it = _runtimeEvents.find(event.as<EventID>());
				if (it == _runtimeEvents.end())
				{
					scriptEngine.ThrowError(std::format("Event not found"));
					return;
				}
				eventInstance = it->second.get();
			}
			else if (event.is<sol::string_view>())
			{
				auto &&it = _runtimeEvents.find(_eventName2ID[event.as<std::string_view>()]);
				if (it == _runtimeEvents.end())
				{
					scriptEngine.ThrowError(std::format("Event not found"));
					return;
				}
				eventInstance = it->second.get();
			}

			EventHandleKey k{nullptr};
			if (key.is<double>())
			{
				k.value = key.as<double>();
			}
			else if (key.is<sol::string_view>())
			{
				k.value = key.as<std::string>();
			}
			eventInstance->Invoke(args, k);
		}
		catch (const std::exception &e)
		{
			UnhandledCppException(_log, e);
			throw;
		}
	});

	scriptEngine.SetReadonlyGlobal("Events", table);
}

void EventManager::UninstallFromScriptEngine(ScriptEngine &scriptEngine)
{
}

void EventManager::AttachToScriptLoader(ScriptLoader &scriptLoader) noexcept
{

	_onPreLoadAllScriptsHandlerID = scriptLoader.onPreLoadAllScripts += [this]()
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

	_onPostLoadAllScriptsHandlerID = scriptLoader.onPostLoadAllScripts += [this]()
	{
		OnScriptsLoaded();
	};

	_onPreHotReloadScriptsHandlerID = scriptLoader.onPreHotReloadScripts += [this](const std::vector<ScriptID> &scriptIDs)
	{
		if (scriptIDs.empty())
		{
			return;
		}

		ClearInvalidScriptEvents();

		for (auto &&[_, event] : _runtimeEvents)
		{
			event->ClearInvalidScriptsHandlers(modManager.scriptProvider);
		}
	};

	_onPreHotReloadScriptsHandlerID = scriptLoader.onPostHotReloadScripts += [this](const std::vector<ScriptID> &scriptIDs)
	{
		OnScriptsLoaded();
	};

	_onUnloadScriptHandlerID = scriptLoader.onUnloadScript += [this](ScriptID scriptID)
	{
		for (auto &&it = _runtimeEvents.begin(); it != _runtimeEvents.end();)
		{
			if (it->second->GetScriptID() == scriptID)
			{
				_log->Trace("Remove script <{}> event \"{}\"", scriptID, _eventID2Name[it->second->GetID()]);
				DeallocEventID(it->first);
				it = _runtimeEvents.erase(it);
			}
			else
			{
				++it;
			}
		}

		for (auto &&[_, event] : _runtimeEvents)
		{
			event->ClearSpecificScriptHandlers(modManager.scriptProvider, scriptID);
		}
	};
}

void EventManager::DetachFromScriptLoader(ScriptLoader &scriptLoader) noexcept
{
	scriptLoader.onPreLoadAllScripts -= _onPreLoadAllScriptsHandlerID;
	scriptLoader.onPostLoadAllScripts -= _onPostLoadAllScriptsHandlerID;
	scriptLoader.onPreHotReloadScripts -= _onPreHotReloadScriptsHandlerID;
	scriptLoader.onPostHotReloadScripts -= _onPreHotReloadScriptsHandlerID;
	scriptLoader.onUnloadScript -= _onUnloadScriptHandlerID;
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
	for (auto &&it = _runtimeEvents.begin(); it != _runtimeEvents.end();)
	{
		auto &&scriptID = it->second->GetScriptID();
		if (scriptID && !modManager.scriptProvider.IsValidScriptID(scriptID))
		{
			DeallocEventID(it->first);
			_log->Trace("Clear invalid event <{}> from script <{}>", it->first, scriptID);
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
	if (!eventID)
	{
		return std::nullopt;
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
	return std::nullopt;
}

std::unordered_map<EventID, std::shared_ptr<RuntimeEvent>>::const_iterator EventManager::BeginRuntimeEvents() const
{
	return _runtimeEvents.begin();
}

std::unordered_map<EventID, std::shared_ptr<RuntimeEvent>>::const_iterator EventManager::EndRuntimeEvents() const
{
	return _runtimeEvents.end();
}
