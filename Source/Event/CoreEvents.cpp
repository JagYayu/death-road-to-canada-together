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

#include "Event/CoreEvents.hpp"

#include "Event/EventManager.hpp"
#include "Event/RuntimeEvent.hpp"
#include "Util/MicrosImpl.hpp"

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
	TE_CORE_EVENTS_GEN_INIT(_keyboardPress, "KeyboardPress");
	TE_CORE_EVENTS_GEN_INIT(_keyboardRelease, "KeyboardRelease");
	TE_CORE_EVENTS_GEN_INIT(_keyboardRepeat, "KeyboardRepeat");
	TE_CORE_EVENTS_GEN_INIT(_keyboardAdded, "KeyboardAdded");
	TE_CORE_EVENTS_GEN_INIT(_keyboardRemoved, "KeyboardRemoved");
	TE_CORE_EVENTS_GEN_INIT(_mouseMotion, "MouseMotion");
	TE_CORE_EVENTS_GEN_INIT(_mousePress, "MousePress");
	TE_CORE_EVENTS_GEN_INIT(_mouseRelease, "MouseRelease");
	TE_CORE_EVENTS_GEN_INIT(_mouseWheel, "MouseWheel");
	TE_CORE_EVENTS_GEN_INIT(_mouseAdded, "MouseAdded");
	TE_CORE_EVENTS_GEN_INIT(_mouseRemoved, "MouseRemoved");
	TE_CORE_EVENTS_GEN_INIT(_scriptGlobalIndex, "ScriptGlobalIndex");
	TE_CORE_EVENTS_GEN_INIT(_scriptUnload, "ScriptUnload");
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
TE_CORE_EVENTS_GEN_GETTER(KeyboardPress, _keyboardPress);
TE_CORE_EVENTS_GEN_GETTER(KeyboardRelease, _keyboardRelease);
TE_CORE_EVENTS_GEN_GETTER(KeyboardRepeat, _keyboardRepeat);
TE_CORE_EVENTS_GEN_GETTER(KeyboardAdded, _keyboardAdded);
TE_CORE_EVENTS_GEN_GETTER(KeyboardRemoved, _keyboardRemoved);
TE_CORE_EVENTS_GEN_GETTER(MouseMotion, _mouseMotion);
TE_CORE_EVENTS_GEN_GETTER(MousePress, _mousePress);
TE_CORE_EVENTS_GEN_GETTER(MouseRelease, _mouseRelease);
TE_CORE_EVENTS_GEN_GETTER(MouseWheel, _mouseWheel);
TE_CORE_EVENTS_GEN_GETTER(MouseAdded, _mouseAdded);
TE_CORE_EVENTS_GEN_GETTER(MouseRemoved, _mouseRemoved);
TE_CORE_EVENTS_GEN_GETTER(ScriptGlobalIndex, _scriptGlobalIndex);
TE_CORE_EVENTS_GEN_GETTER(ScriptUnload, _scriptUnload);
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
TE_CORE_EVENTS_GEN_GETTER(WindowResize, _windowResize);
