#include "event/CoreEvents.hpp"
#include "event/EventManager.hpp"
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
	TE_CORE_EVENTS_GEN_INIT(_debugCommand, "DebugCommand");
	TE_CORE_EVENTS_GEN_INIT(_debugSnapshot, "DebugSnapshot");
	TE_CORE_EVENTS_GEN_INIT(_keyDown, "KeyDown");
	TE_CORE_EVENTS_GEN_INIT(_keyUp, "KeyUp");
	TE_CORE_EVENTS_GEN_INIT(_mouseMove, "MouseMove");
	TE_CORE_EVENTS_GEN_INIT(_mouseButtonDown, "MouseButtonDown");
	TE_CORE_EVENTS_GEN_INIT(_mouseButtonUp, "MouseButtonUp");
	TE_CORE_EVENTS_GEN_INIT(_mouseWheel, "MouseWheel");
	TE_CORE_EVENTS_GEN_INIT(_serverConnect, "ServerConnect");
	TE_CORE_EVENTS_GEN_INIT(_serverDisconnect, "ServerDisconnect");
	TE_CORE_EVENTS_GEN_INIT(_serverMessage, "ServerMessage");
	TE_CORE_EVENTS_GEN_INIT(_tickLoad, "TickLoad");
	TE_CORE_EVENTS_GEN_INIT(_tickRender, "TickRender");
	TE_CORE_EVENTS_GEN_INIT(_tickUpdate, "TickUpdate");
}

#define TUDOV_CORE_EVENTS_GEN_GETTER(Func, Field) \
	RuntimeEvent &CoreEvents::Func() noexcept     \
	{                                             \
		return *Field;                            \
	}                                             \
	TE_GEN_END

TUDOV_CORE_EVENTS_GEN_GETTER(ClientConnect, _clientConnect);
TUDOV_CORE_EVENTS_GEN_GETTER(ClientDisconnect, _clientDisconnect);
TUDOV_CORE_EVENTS_GEN_GETTER(ClientMessage, _clientMessage);
TUDOV_CORE_EVENTS_GEN_GETTER(DebugCommand, _debugCommand);
TUDOV_CORE_EVENTS_GEN_GETTER(DebugSnapshot, _debugSnapshot);
TUDOV_CORE_EVENTS_GEN_GETTER(KeyDown, _keyDown);
TUDOV_CORE_EVENTS_GEN_GETTER(KeyUp, _keyUp);
TUDOV_CORE_EVENTS_GEN_GETTER(MouseMove, _mouseMove);
TUDOV_CORE_EVENTS_GEN_GETTER(MouseButtonDown, _mouseButtonDown);
TUDOV_CORE_EVENTS_GEN_GETTER(MouseButtonUp, _mouseButtonUp);
TUDOV_CORE_EVENTS_GEN_GETTER(MouseWheel, _mouseWheel);
TUDOV_CORE_EVENTS_GEN_GETTER(ServerConnect, _serverConnect);
TUDOV_CORE_EVENTS_GEN_GETTER(ServerDisconnect, _serverDisconnect);
TUDOV_CORE_EVENTS_GEN_GETTER(ServerMessage, _serverMessage);
TUDOV_CORE_EVENTS_GEN_GETTER(TickLoad, _tickLoad);
TUDOV_CORE_EVENTS_GEN_GETTER(TickUpdate, _tickUpdate);
TUDOV_CORE_EVENTS_GEN_GETTER(TickRender, _tickRender);
