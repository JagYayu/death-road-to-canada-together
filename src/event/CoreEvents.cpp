#include "CoreEvents.hpp"

#include "EventManager.hpp"
#include "util/MicrosImpl.hpp"

#include <memory>

using namespace tudov;

#define TUDOV_CORE_EVENTS_GEN_INIT(Field, Name)                                            \
	Field = std::make_shared<RuntimeEvent>(eventManager, eventManager.AllocEventID(Name)); \
	eventManager._runtimeEvents.try_emplace(Field->GetID(), Field)

CoreEvents::CoreEvents(EventManager &eventManager) noexcept
{
	TUDOV_CORE_EVENTS_GEN_INIT(_clientConnect, "ClientConnect");
	TUDOV_CORE_EVENTS_GEN_INIT(_clientDisconnect, "ClientDisconnect");
	TUDOV_CORE_EVENTS_GEN_INIT(_clientMessage, "ClientMessage");
	TUDOV_CORE_EVENTS_GEN_INIT(_keyDown, "KeyDown");
	TUDOV_CORE_EVENTS_GEN_INIT(_keyUp, "KeyUp");
	TUDOV_CORE_EVENTS_GEN_INIT(_mouseMove, "MouseMove");
	TUDOV_CORE_EVENTS_GEN_INIT(_mouseButtonDown, "MouseButtonDown");
	TUDOV_CORE_EVENTS_GEN_INIT(_mouseButtonUp, "MouseButtonUp");
	TUDOV_CORE_EVENTS_GEN_INIT(_mouseWheel, "MouseWheel");
	TUDOV_CORE_EVENTS_GEN_INIT(_serverConnect, "ServerConnect");
	TUDOV_CORE_EVENTS_GEN_INIT(_serverDisconnect, "ServerDisconnect");
	TUDOV_CORE_EVENTS_GEN_INIT(_serverMessage, "ServerMessage");
	TUDOV_CORE_EVENTS_GEN_INIT(_tickLoad, "TickLoad");
	TUDOV_CORE_EVENTS_GEN_INIT(_tickRender, "TickRender");
	TUDOV_CORE_EVENTS_GEN_INIT(_tickUpdate, "TickUpdate");
}

#define TUDOV_CORE_EVENTS_GEN_GETTER(Func, Field)         \
	CoreEvents::TRuntimeEvent CoreEvents::Func() noexcept \
	{                                                     \
		return Field;                                     \
	}                                                     \
	TUDOV_GEN_END

TUDOV_CORE_EVENTS_GEN_GETTER(ClientConnect, _clientConnect);
TUDOV_CORE_EVENTS_GEN_GETTER(ClientDisconnect, _clientDisconnect);
TUDOV_CORE_EVENTS_GEN_GETTER(ClientMessage, _clientMessage);
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
