#include "EventManager.h"

#include "AbstractEvent.h"
#include "RuntimeEvent.h"
#include "mod/ScriptLoader.h"
#include "program/Engine.h"
#include "util/Defs.h"

using namespace tudov;

EventManager::EventManager(Engine &engine)
    : _engine(engine),
      _staticEvents{update, render},
      update("Update"),
      render("Render")
{
}

EventManager::~EventManager()
{
	if (_initialized)
	{
		_engine.modManager.scriptEngine.scriptLoader.onPreLoadAllScripts -= _onPreLoadScriptHandlerID;
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

void EventManager::OnPreLoadScript(const StringView &scriptName)
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

	_onPreLoadScriptHandlerID = _engine.modManager.scriptEngine.scriptLoader.onPreLoadAllScripts += [&]()
	{
		OnPreLoadScripts();
	};

	auto &&lua = _engine.modManager.scriptEngine.GetState();

	auto &&usertype = lua.new_usertype<EventManager>("EventManager");

	_initialized = true;
}

Optional<Reference<AbstractEvent>> EventManager::TryGetRegistryEvent(const StringView &eventName)
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
