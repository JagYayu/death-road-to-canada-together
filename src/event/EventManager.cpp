#include "EventManager.h"

#include "AbstractEvent.h"
#include "RuntimeEvent.h"
#include "mod/ModManager.h"
#include "mod/ScriptEngine.h"
#include "mod/ScriptLoader.h"
#include "sol/forward.hpp"
#include "util/Defs.h"
#include "util/Utils.hpp"

using namespace tudov;

EventManager::EventManager(ModManager &modManager)
    : modManager(modManager),
      _log(Log::Get("EventManager")),
      _latestEventID(),
      update(MakeShared<RuntimeEvent>(AllocEventID("Update"))),
      render(MakeShared<RuntimeEvent>(AllocEventID("Render")))
{
	_runtimeEvents.emplace(update->GetID(), update);
	_runtimeEvents.emplace(render->GetID(), render);
}

EventManager::~EventManager()
{
}

EventID EventManager::AllocEventID(StringView eventName) noexcept
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
	auto &&name = _eventID2Name.emplace(id, String(eventName)).first->second;
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
	Vector<Reference<LoadtimeEvent>> invalidEvents;

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
			auto &&runtimeEvent = MakeShared<RuntimeEvent>(loadtimeEvent->ToRuntime());
			_runtimeEvents.emplace(runtimeEvent->GetID(), runtimeEvent);
		}
	}

	_loadtimeEvents = {};
}

void EventManager::Initialize()
{
	auto &&table = modManager.scriptEngine.CreateTable(0);

	table["add"] = [&](const sol::table &args)
	{
		try
		{
			auto &&loadingScript = modManager.scriptEngine.scriptLoader.GetLoadingScript();
			if (!loadingScript)
			{
				modManager.scriptEngine.ThrowError("Failed to add event handler: must add at script load time");
				return;
			}

			auto &&argEvent = args["event"];
			if (!argEvent.valid() || !argEvent.is<sol::string_view>())
			{
				modManager.scriptEngine.ThrowError(Format("Failed to add event handler: invalid `event` type, expected string, got {}", GetLuaTypeStringView(argEvent.get_type())));
				return;
			}
			auto &&argFunc = args["func"];
			if (!argFunc.valid() || !argFunc.is<sol::function>())
			{
				modManager.scriptEngine.ThrowError(Format("Failed to add event handler: invalid `func` type, expected function, got {}", GetLuaTypeStringView(argFunc.get_type())));
				return;
			}

			auto &&eventName = argEvent.get<sol::string_view>();
			auto &&eventID = GetEventIDByName(eventName);
			auto &&event = TryGetRegistryEvent(eventID);
			if (!event.has_value())
			{
				modManager.scriptEngine.ThrowError(Format("Failed to add event handler: event named '{}' not found", eventName));
				return;
			}

			auto &&argOrder = args["order"];
			Optional<String> order;
			if (!argOrder.valid() || argOrder.is<sol::nil_t>())
			{
				order = null;
			}
			else if (argOrder.is<sol::string_view>())
			{
				order = argOrder.get<sol::string_view>();
			}
			else
			{
				modManager.scriptEngine.ThrowError(Format("Failed to add event handler: invalid `order` type, nil or string expected, got {}", GetLuaTypeStringView(argOrder.get_type())));
				return;
			}

			auto &&argKey = args["key"];
			Optional<AddHandlerArgs::Key> key;
			if (!argKey.valid() || argKey.is<sol::nil_t>())
			{
				key = null;
			}
			else if (argKey.is<Number>())
			{
				key = AddHandlerArgs::Key(argKey.get<Number>());
			}
			else if (argKey.is<sol::string_view>())
			{
				key = AddHandlerArgs::Key(String(argKey.get<sol::string_view>()));
			}
			else
			{
				modManager.scriptEngine.ThrowError(Format("Failed to add event handler: invalid `key` type, nil or number or string expected, got {}", GetLuaTypeStringView(argKey.get_type())));
				return;
			}

			auto &&argSequence = args["sequence"];
			Optional<Number> sequence;
			if (!argSequence.valid() || argSequence.is<sol::nil_t>())
			{
				sequence = null;
			}
			else if (argSequence.is<Number>())
			{
				sequence = argSequence.get<Number>();
			}
			else
			{
				modManager.scriptEngine.ThrowError(Format("Failed to add event handler: invalid `sequence` type, nil or string expected, got {}", GetLuaTypeStringView(argOrder.get_type())));
				return;
			}

			event->get().Add(AddHandlerArgs{
			    .eventID = eventID,
			    .scriptID = loadingScript,
			    .function = EventHandler::Function(argFunc.get<sol::function>()), // {argEvent.get<sol::function>()},
			    .order = Move(order),
			    .key = Move(key),
			    .sequence = Move(sequence),
			});
		}
		catch (const std::exception &e)
		{
			UnhandledCppException(_log, e);
			throw;
		}
	};

	modManager.scriptEngine.Set("Events", modManager.scriptEngine.MakeReadonlyGlobal(table));

	_onPreLoadAllScriptsHandlerID = modManager.scriptEngine.scriptLoader.onPreLoadAllScripts += [&]()
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

	_onPostLoadAllScriptsHandlerID = modManager.scriptEngine.scriptLoader.onPostLoadAllScripts += [&]()
	{
		OnScriptsLoaded();
	};

	_onPreHotReloadScriptsHandlerID = modManager.scriptEngine.scriptLoader.onPreHotReloadScripts += [this](const Vector<ScriptID> &scriptIDs)
	{
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

	_onPreHotReloadScriptsHandlerID = modManager.scriptEngine.scriptLoader.onPostHotReloadScripts += [&](const Vector<ScriptID> &scriptIDs)
	{
		OnScriptsLoaded();
	};
}

void EventManager::Deinitialize()
{
	modManager.scriptEngine.scriptLoader.onPreLoadAllScripts -= _onPreLoadAllScriptsHandlerID;
	modManager.scriptEngine.scriptLoader.onPostLoadAllScripts -= _onPostLoadAllScriptsHandlerID;
	modManager.scriptEngine.scriptLoader.onPreHotReloadScripts -= _onPreHotReloadScriptsHandlerID;
	modManager.scriptEngine.scriptLoader.onPostHotReloadScripts -= _onPreHotReloadScriptsHandlerID;
}

EventID EventManager::GetEventIDByName(StringView eventName) const noexcept
{
	auto &&it = _eventName2ID.find(String(eventName));
	if (it == _eventName2ID.end())
	{
		return false;
	}
	return it->second;
}

Optional<StringView> EventManager::GetEventNameByID(EventID eventID) const noexcept
{
	auto &&it = _eventID2Name.find(eventID);
	if (it == _eventID2Name.end())
	{
		return null;
	}
	return it->second;
}

bool EventManager::IsValidEventID(EventID eventID) const noexcept
{
	return _eventID2Name.contains(eventID);
}

Optional<Reference<AbstractEvent>> EventManager::TryGetRegistryEvent(EventID eventID)
{
	if (!eventID)
	{
		return null;
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
	return null;
}
