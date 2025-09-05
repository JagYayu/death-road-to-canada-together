/**
 * @file event/CoreEvents.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "event/CoreEvents.hpp"

#include "event/EventManager.hpp"
#include "event/RuntimeEvent.hpp"
#include "util/MicrosImpl.hpp"

#include <memory>

using namespace tudov;

#define TE_CORE_EVENTS_GEN_INIT(Field, Name)                                               \
	Field = std::make_shared<RuntimeEvent>(eventManager, eventManager.AllocEventID(Name)); \
	eventManager._runtimeEvents.try_emplace(Field->GetID(), Field)

CoreEvents::CoreEvents(EventManager &eventManager) noexcept
{
	TE_CORE_EVENTS_GEN_INIT(_clientConnect, "ClientConnect");
	TE_CORE_EVENTS_GEN_INIT(_clientDisconnect, "ClientDisconnect");
	TE_CORE_EVENTS_GEN_INIT(_clientMessage, "ClientMessage");
	TE_CORE_EVENTS_GEN_INIT(_debugProvide, "DebugProvide");
	TE_CORE_EVENTS_GEN_INIT(_debugSnapshot, "DebugSnapshot");
	TE_CORE_EVENTS_GEN_INIT(_keyDown, "KeyDown");
	TE_CORE_EVENTS_GEN_INIT(_keyRepeat, "KeyRepeat");
	TE_CORE_EVENTS_GEN_INIT(_keyUp, "KeyUp");
	TE_CORE_EVENTS_GEN_INIT(_modGlobalsIndex, "ScriptGlobalIndex");
	TE_CORE_EVENTS_GEN_INIT(_mouseMove, "MouseMove");
	TE_CORE_EVENTS_GEN_INIT(_mouseButtonDown, "MouseButtonDown");
	TE_CORE_EVENTS_GEN_INIT(_mouseButtonUp, "MouseButtonUp");
	TE_CORE_EVENTS_GEN_INIT(_mouseWheel, "MouseWheel");
	TE_CORE_EVENTS_GEN_INIT(_scriptsLoaded, "ScriptsLoaded");
	TE_CORE_EVENTS_GEN_INIT(_serverAuthenticate, "ServerAuthenticate");
	TE_CORE_EVENTS_GEN_INIT(_serverConnect, "ServerConnect");
	TE_CORE_EVENTS_GEN_INIT(_serverDisconnect, "ServerDisconnect");
	TE_CORE_EVENTS_GEN_INIT(_serverHost, "ServerHost");
	TE_CORE_EVENTS_GEN_INIT(_serverMessage, "ServerMessage");
	TE_CORE_EVENTS_GEN_INIT(_serverShutdown, "ServerShutdown");
	TE_CORE_EVENTS_GEN_INIT(_tickLoad, "TickLoad");
	TE_CORE_EVENTS_GEN_INIT(_tickRender, "TickRender");
	TE_CORE_EVENTS_GEN_INIT(_tickUpdate, "TickUpdate");
}

#define TE_CORE_EVENTS_GEN_GETTER(Func, Field) \
	RuntimeEvent &CoreEvents::Func() noexcept  \
	{                                          \
		return *Field;                         \
	}                                          \
	TE_MICRO_END

TE_CORE_EVENTS_GEN_GETTER(ClientConnect, _clientConnect);
TE_CORE_EVENTS_GEN_GETTER(ClientDisconnect, _clientDisconnect);
TE_CORE_EVENTS_GEN_GETTER(ClientMessage, _clientMessage);
TE_CORE_EVENTS_GEN_GETTER(DebugProvide, _debugProvide);
TE_CORE_EVENTS_GEN_GETTER(DebugSnapshot, _debugSnapshot);
TE_CORE_EVENTS_GEN_GETTER(KeyDown, _keyDown);
TE_CORE_EVENTS_GEN_GETTER(KeyRepeat, _keyRepeat);
TE_CORE_EVENTS_GEN_GETTER(KeyUp, _keyUp);
TE_CORE_EVENTS_GEN_GETTER(ModGlobalsIndex, _modGlobalsIndex);
TE_CORE_EVENTS_GEN_GETTER(MouseMove, _mouseMove);
TE_CORE_EVENTS_GEN_GETTER(MouseButtonDown, _mouseButtonDown);
TE_CORE_EVENTS_GEN_GETTER(MouseButtonUp, _mouseButtonUp);
TE_CORE_EVENTS_GEN_GETTER(MouseWheel, _mouseWheel);
TE_CORE_EVENTS_GEN_GETTER(ScriptsLoaded, _scriptsLoaded);
TE_CORE_EVENTS_GEN_GETTER(ServerAuthenticate, _serverAuthenticate);
TE_CORE_EVENTS_GEN_GETTER(ServerConnect, _serverConnect);
TE_CORE_EVENTS_GEN_GETTER(ServerDisconnect, _serverDisconnect);
TE_CORE_EVENTS_GEN_GETTER(ServerHost, _serverHost);
TE_CORE_EVENTS_GEN_GETTER(ServerMessage, _serverMessage);
TE_CORE_EVENTS_GEN_GETTER(ServerShutdown, _serverShutdown);
TE_CORE_EVENTS_GEN_GETTER(TickLoad, _tickLoad);
TE_CORE_EVENTS_GEN_GETTER(TickUpdate, _tickUpdate);
TE_CORE_EVENTS_GEN_GETTER(TickRender, _tickRender);
