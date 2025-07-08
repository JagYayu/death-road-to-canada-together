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
#include <stdexcept>
#include <string_view>

using namespace tudov;

EventManager::EventManager(Context &context) noexcept
    : _context(context),
      _log(Log::Get("EventManager")),
      _latestEventID(),
      _update(std::make_shared<RuntimeEvent>(*this, AllocEventID("Update"))),
      _render(std::make_shared<RuntimeEvent>(*this, AllocEventID("Render"))),
      _keyDown(std::make_shared<RuntimeEvent>(*this, AllocEventID("KeyDown"))),
      _keyUp(std::make_shared<RuntimeEvent>(*this, AllocEventID("KeyUp"))),
      _mouseMove(std::make_shared<RuntimeEvent>(*this, AllocEventID("MouseMove"))),
      _mouseButtonDown(std::make_shared<RuntimeEvent>(*this, AllocEventID("MouseButtonDown"))),
      _mouseButtonUp(std::make_shared<RuntimeEvent>(*this, AllocEventID("MouseButtonUp"))),
      _mouseWheel(std::make_shared<RuntimeEvent>(*this, AllocEventID("MouseWheel")))
{
	InitRuntimeEvent(_update);
	InitRuntimeEvent(_render);
	InitRuntimeEvent(_keyDown);
	InitRuntimeEvent(_keyUp);
	InitRuntimeEvent(_mouseMove);
	InitRuntimeEvent(_mouseButtonDown);
	InitRuntimeEvent(_mouseButtonUp);
	InitRuntimeEvent(_mouseWheel);
}

void EventManager::InitRuntimeEvent(std::shared_ptr<RuntimeEvent> &event) noexcept
{
	_runtimeEvents.emplace(event->GetID(), event);
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
		if (!eventID) [[unlikely]]
		{
			throw std::runtime_error("Loadtime events contains event with invalid id!");
		}

		if (_runtimeEvents.contains(eventID))
		{
			auto &&msg = std::format("duplicated event <{}>\"{}\"", eventID, GetEventNameByID(eventID)->data());
			throw std::runtime_error(msg);
		}

		if (loadtimeEvent->IsBuilt())
		{
			auto &&runtimeEvent = std::make_shared<RuntimeEvent>(loadtimeEvent->ToRuntime());
			if (eventID != runtimeEvent->GetID())
			{
				_log->Error("Converted runtime event has different id to previous loadtime event! Expected <{}> but got <{}>", eventID, runtimeEvent->GetID());
			}
			else
			{
				_log->Trace("Loadtime event converted to a runtime event <{}>\"{}\"", eventID, GetEventNameByID(eventID)->data());
				assert(_runtimeEvents.try_emplace(eventID, runtimeEvent).second);
			}
		}
		else
		{
			auto &&eventName = _eventID2Name[eventID];
			auto scriptID = loadtimeEvent->GetScriptID();
			auto scriptName = GetScriptProvider()->GetScriptNameByID(scriptID).value_or("$UNKNOWN$");
			_log->Error("Attempt to add handlers to non-exist event <{}>\"{}\", source script <{}>\"{}\"", eventID, eventName, scriptID, scriptName);
		}
	}

	_loadtimeEvents.clear();
	_log->Trace("Cleared loadtime events");

	auto &&scriptLoader = GetScriptLoader();
	auto &&scriptProvider = GetScriptProvider();

	for (auto &&[_, event] : _runtimeEvents)
	{
		auto scriptID = event->GetScriptID();
		assert(!scriptID || scriptProvider->IsValidScript(scriptID) && "Invalid script detected!");

		if (scriptID)
		{
			for (auto &&it = event->BeginHandlers(); it != event->EndHandlers(); ++it)
			{
				scriptLoader->AddReverseDependency(it->scriptID, scriptID);
			}
		}
	}
}

Context &EventManager::GetContext() noexcept
{
	return _context;
}

void EventManager::Initialize() noexcept
{
	auto &&scriptLoader = GetScriptLoader();

	_onPreLoadAllScriptsHandlerID = scriptLoader->GetOnPreLoadAllScripts() += [this]()
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

	_onPostLoadAllScriptsHandlerID = scriptLoader->GetOnPostLoadAllScripts() += [this]()
	{
		OnScriptsLoaded();
	};

	_onPreHotReloadScriptsHandlerID = scriptLoader->GetOnPreHotReloadScripts() += [this](const std::vector<ScriptID> &scriptIDs)
	{
		if (scriptIDs.empty())
		{
			return;
		}

		ClearInvalidScriptEvents();

		auto &&scriptProvider = *GetScriptProvider();
		for (auto &&[_, event] : _runtimeEvents)
		{
			event->ClearInvalidScriptsHandlers(scriptProvider);
		}
	};

	_onPostHotReloadScriptsHandlerID = scriptLoader->GetOnPostHotReloadScripts() += [this](const std::vector<ScriptID> &scriptIDs)
	{
		OnScriptsLoaded();
	};

	_onUnloadScriptHandlerID = scriptLoader->GetOnUnloadScript() += [this](ScriptID scriptID)
	{
		assert(scriptID && "invalid script id!");

		for (auto &&it = _runtimeEvents.begin(); it != _runtimeEvents.end();)
		{
			if (it->second->GetScriptID() == scriptID)
			{
				auto eventID = it->second->GetID();
				_log->Trace("Remove event <{}>\"{}\" from script <{}>", eventID, _eventID2Name[eventID], scriptID);
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
			event->ClearSpecificScriptHandlers(*scriptProvider, scriptID);
		}
	};
}

void EventManager::Deinitialize() noexcept
{
	auto &&scriptLoader = GetScriptLoader();

	scriptLoader->GetOnPreLoadAllScripts() -= _onPreLoadAllScriptsHandlerID;
	scriptLoader->GetOnPostLoadAllScripts() -= _onPostLoadAllScriptsHandlerID;
	scriptLoader->GetOnPreHotReloadScripts() -= _onPreHotReloadScriptsHandlerID;
	scriptLoader->GetOnPostHotReloadScripts() -= _onPreHotReloadScriptsHandlerID;
	scriptLoader->GetOnUnloadScript() -= _onUnloadScriptHandlerID;
}

void EventManager::InstallToScriptEngine(IScriptEngine &scriptEngine)
{
}

void EventManager::UninstallFromScriptEngine(IScriptEngine &scriptEngine)
{
}

void EventManager::LuaAdd(const sol::object &event, const sol::object &func, const sol::object &name, const sol::object &order, const sol::object &key, const sol::object &sequence)
{
	try
	{
		auto &&scriptEngine = GetScriptEngine();

		auto scriptID = GetScriptLoader()->GetLoadingScriptID();
		if (!scriptID)
		{
			scriptEngine->ThrowError("Failed to add event handler: must add at script load time");
			return;
		}

		if (!event.valid() || !event.is<sol::string_view>())
		{
			scriptEngine->ThrowError(std::format("Failed to add event handler: invalid arg#1 `event` type, expected string, got {}", GetLuaTypeStringView(event.get_type())));
			return;
		}
		std::string_view eventName = event.as<sol::string_view>();

		if (!func.valid() || !func.is<sol::function>())
		{
			scriptEngine->ThrowError(std::format("Failed to add event handler: invalid arg#2 `func` type, expected function, got {}", GetLuaTypeStringView(func.get_type())));
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
			scriptEngine->ThrowError(std::format("Failed to add event handler: invalid arg#3 `name` type, nil or string expected, got {}", GetLuaTypeStringView(name.get_type())));
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
			scriptEngine->ThrowError(std::format("Failed to add event handler: invalid arg#4 `order` type, nil or string expected, got {}", GetLuaTypeStringView(order.get_type())));
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
			scriptEngine->ThrowError(std::format("Failed to add event handler: invalid arg#5 `key` type, nil or number or number expected, got {}", GetLuaTypeStringView(key.get_type())));
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
			scriptEngine->ThrowError(std::format("Failed to add event handler: invalid arg#6 `sequence` type, nil or string expected, got {}", GetLuaTypeStringView(sequence.get_type())));
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
			auto &&loadtimeEvent = std::make_shared<LoadtimeEvent>(*this, eventID, scriptID);
			_log->Trace("Script <{}> added handler to loadtime event <{}>\"{}\"", scriptID, eventID, eventName);
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
		});
	}
	catch (const std::exception &e)
	{
		UnhandledCppException(_log, e);
		throw;
	}
}

EventID EventManager::LuaNew(const sol::object &event, const sol::object &orders, const sol::object &keys)
{
	try
	{
		auto &&scriptEngine = GetScriptEngine();

		auto &&scriptID = GetScriptLoader()->GetLoadingScriptID();
		if (!scriptID)
		{
			scriptEngine->ThrowError("Failed to new event: must call at script load time");
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
				_log->Trace("Script <{}> alloc event <{}>\"{}\"", scriptID, eventID, eventName);
			}
		}
		else
		{
			scriptEngine->ThrowError(std::format("Failed to new event: invalid arg#1 `event` type, string expected, got {}", GetLuaTypeStringView(event.get_type())));
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
					scriptEngine->ThrowError(std::format("Failed to new event: invalid arg#2 'orders' type, table must be a string array, contains {}", GetLuaTypeStringView(tbl[i].get_type())));
					return false;
				}
				orders_.emplace_back(val.as<sol::string_view>());
			}
		}
		else
		{
			scriptEngine->ThrowError(std::format("Failed to new event: invalid arg#2 'orders' type, table expected, got {}", GetLuaTypeStringView(orders.get_type())));
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
					scriptEngine->ThrowError(std::format("Failed to new event: invalid arg#2 'keys' type, table must be a number/string array, contains {}", GetLuaTypeStringView(tbl[i].get_type())));
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
			scriptEngine->ThrowError(std::format("Failed to new event: invalid arg#2 'keys' type, table or nil expected, got {}", GetLuaTypeStringView(keys.get_type())));
			return false;
		}

		bool dup = false;
		{
			auto &&it = _loadtimeEvents.find(eventID);
			if (it == _loadtimeEvents.end())
			{
				auto &&loadtimeEvent = std::make_shared<LoadtimeEvent>(*this, eventID, scriptID, orders_, keys_);
				_loadtimeEvents.try_emplace(eventID, loadtimeEvent);
			}
			else if (!it->second->TryBuild(scriptID, orders_, keys_))
			{
				dup = true;
			}
		}
		if (dup)
		{
			scriptEngine->ThrowError(std::format("Failed to new event <{}>\"{}\", already been registered", eventID, eventName));
			return false;
		}

		_log->Trace("Constructed loadtime event <{}>\"{}\" from script <{}>", eventID, eventName, scriptID);
		return eventID;
	}
	catch (const std::exception &e)
	{
		UnhandledCppException(_log, e);
		throw;
	}
}

void EventManager::LuaInvoke(const sol::object &event, const sol::object &args, const sol::object &key, const sol::object &uncached)
{
	try
	{
		auto &&scriptEngine = GetScriptEngine();

		RuntimeEvent *eventInstance;
		if (event.is<EventID>())
		{
			auto eventID = event.as<EventID>();
			auto &&it = _runtimeEvents.find(eventID);
			if (it == _runtimeEvents.end())
			{
				scriptEngine->ThrowError(std::format("Runtime event <{}> not found", eventID));
				return;
			}
			eventInstance = it->second.get();
		}
		else if (event.is<sol::string_view>())
		{
			auto eventName = event.as<std::string_view>();
			auto &&it = _runtimeEvents.find(_eventName2ID[eventName]);
			if (it == _runtimeEvents.end())
			{
				scriptEngine->ThrowError(std::format("Runtime event <{}> not found", eventName));
				return;
			}
			eventInstance = it->second.get();
		}
		else
		{
			scriptEngine->ThrowError("Bad argument #1 to 'event': expected EventID or string");
			return;
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

		assert(eventInstance);

		if (!uncached.valid() || uncached.is<bool>() && !uncached.as<bool>()) [[unlikely]]
		{
			eventInstance->InvokeUncached(args, k);
		}
		else [[likely]]
		{
			eventInstance->Invoke(args, k);
		}
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
		if (scriptID && !scriptProvider->IsValidScript(scriptID))
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

std::unordered_map<EventID, std::shared_ptr<RuntimeEvent>>::const_iterator EventManager::BeginRuntimeEvents() const
{
	return _runtimeEvents.begin();
}

std::unordered_map<EventID, std::shared_ptr<RuntimeEvent>>::const_iterator EventManager::EndRuntimeEvents() const
{
	return _runtimeEvents.end();
}

RuntimeEvent &EventManager::GetUpdateEvent() noexcept
{
	return *_update;
}

RuntimeEvent &EventManager::GetRenderEvent() noexcept
{
	return *_render;
}

RuntimeEvent &EventManager::GetKeyDownEvent() noexcept
{
	return *_keyDown;
}

RuntimeEvent &EventManager::GetKeyUpEvent() noexcept
{
	return *_keyUp;
}

RuntimeEvent &EventManager::GetMouseMoveEvent() noexcept
{
	return *_mouseMove;
}

RuntimeEvent &EventManager::GetMouseButtonDownEvent() noexcept
{
	return *_mouseButtonDown;
}

RuntimeEvent &EventManager::GetMouseButtonUpEvent() noexcept
{
	return *_mouseButtonUp;
}

RuntimeEvent &EventManager::GetMouseWheelEvent() noexcept
{
	return *_mouseWheel;
}
