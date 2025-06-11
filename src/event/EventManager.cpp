#include "EventManager.h"

#include "AbstractEvent.h"
#include "RuntimeEvent.h"
#include "mod/ScriptEngine.h"
#include "mod/ScriptLoader.h"
#include "program/Engine.h"
#include "util/Defs.h"

using namespace tudov;

void EventManager::RegisterScriptGlobal(ScriptEngine &scriptEngine)
{
	auto &&table = scriptEngine.CreateTable(0);

	table["add"] = [&](const sol::table &args)
	{
		try
		{
			auto &&loadingScript = scriptEngine.scriptLoader.GetLoadingScript();
			if (!loadingScript)
			{
				scriptEngine.ThrowError("Failed to add event handler: must add at script load time");
				return;
			}

			auto &&argEvent = args["event"];
			if (!argEvent.valid() || !argEvent.is<sol::string_view>())
			{
				scriptEngine.ThrowError(Format("Failed to add event handler: invalid `event` type, expected string, got {}", GetLuaTypeStringView(argEvent.get_type())));
				return;
			}
			auto &&argFunc = args["func"];
			if (!argFunc.valid() || !argFunc.is<sol::function>())
			{
				scriptEngine.ThrowError(Format("Failed to add event handler: invalid `func` type, expected function, got {}", GetLuaTypeStringView(argFunc.get_type())));
				return;
			}

			auto &&eventName = argEvent.get<sol::string_view>();
			auto &&event = TryGetRegistryEvent(eventName);
			if (!event.has_value())
			{
				scriptEngine.ThrowError(Format("Failed to add event handler: event named '{}' not found", eventName));
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
				scriptEngine.ThrowError(Format("Failed to add event handler: invalid `order` type, nil or string expected, got {}", GetLuaTypeStringView(argOrder.get_type())));
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
				scriptEngine.ThrowError(Format("Failed to add event handler: invalid `key` type, nil or number or string expected, got {}", GetLuaTypeStringView(argKey.get_type())));
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
				scriptEngine.ThrowError(Format("Failed to add event handler: invalid `sequence` type, nil or string expected, got {}", GetLuaTypeStringView(argOrder.get_type())));
				return;
			}

			event->get().Add(AddHandlerArgs{
			    .scriptName = String(engine.modManager.scriptProvider.GetScriptName(loadingScript).value()),
			    .event = String(eventName),
			    .function = {argEvent.get<sol::function>()},
			    .order = Move(order),
			    .key = Move(key),
			    .sequence = Move(sequence),
			});
		}
		catch (const std::exception &e)
		{
			// OnFatalException(e);
			throw;
		}
	};

	scriptEngine.Set("Events", scriptEngine.MakeReadonlyGlobal(table));
}

EventManager::EventManager(Engine &engine)
    : engine(engine),
      _staticEvents{update, render},
      update("Update"),
      render("Render")
{
}

EventManager::~EventManager()
{
	if (_initialized)
	{
		engine.modManager.scriptEngine.scriptLoader.onPreLoadAllScripts -= _onPreLoadScriptHandlerID;
	}
}

void EventManager::OnPreLoadScripts()
{
	_loadtimeEvents.clear();
	_runtimeEvents.clear();
	for (auto &&event : _staticEvents)
	{
		event.get().ClearScriptsHandlers();
	}
}

void EventManager::OnPreLoadScript(StringView scriptName)
{
	MapRemoveIf(_runtimeEvents, [&](const Pair<StringView, RuntimeEvent> &pair)
	{
		return pair.second.GetScriptName() == scriptName;
	});
}

void EventManager::OnScriptsLoaded()
{
	Vector<Reference<LoadtimeEvent>> invalidEvents;

	for (auto &&[name, loadtimeEvent] : _loadtimeEvents)
	{
		if (_runtimeEvents.contains(name))
		{
			auto &&runtimeEvent = loadtimeEvent.ToRuntime();
			_runtimeEvents.emplace(runtimeEvent.GetName(), runtimeEvent);
		}
	}
}

void EventManager::Initialize()
{
	if (_initialized)
	{
		return;
	}

	_onPreLoadScriptHandlerID = engine.modManager.scriptEngine.scriptLoader.onPreLoadAllScripts += [&]()
	{
		OnPreLoadScripts();
	};

	auto &&lua = engine.modManager.scriptEngine.GetState();

	auto &&usertype = lua.new_usertype<EventManager>("EventManager");

	_initialized = true;
}

Optional<Reference<AbstractEvent>> EventManager::TryGetRegistryEvent(StringView eventName)
{
	{
		auto &&it = _loadtimeEvents.find(eventName);
		if (it != _loadtimeEvents.end())
		{
			return it->second;
		}
	}

	{
		auto &&it = _runtimeEvents.find(eventName);
		if (it != _runtimeEvents.end())
		{
			return it->second;
		}
	}

	return null;
}
