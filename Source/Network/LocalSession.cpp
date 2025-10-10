/**
 * @file network/LocalSession.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "network/LocalSession.hpp"

#include "event/CoreEvents.hpp"
#include "event/CoreEventsData.hpp"
#include "event/EventInvocation.hpp"
#include "event/EventManager.hpp"
#include "event/RuntimeEvent.hpp"
#include "network/DisconnectionCode.hpp"
#include "network/LocalClientSession.hpp"
#include "network/LocalServerSession.hpp"
#include "network/LocalSession.hpp"
#include "network/NetworkManager.hpp"
#include "network/SocketType.hpp"

using namespace tudov;

void LocalSession::Disconnect(LocalClientSession &client, LocalServerSession &server, EDisconnectionCode code) noexcept
{
	// {
	// 	EventLocalClientDisconnectData data{
	// 	    .socketType = ESocketType::Local,
	// 	    .code = code,
	// 	    .clientID = client.GetSessionID(),
	// 	    .clientSlot = client.GetClientSlot(),
	// 	    .serverSlot = server.GetSessionSlot(),
	// 	};
	// 	client.GetEventManager().GetCoreEvents().ClientDisconnect().Invoke(&data, {}, EEventInvocation::None);
	// }

	// {
	// 	EventLocalServerDisconnectData data{
	// 	    .socketType = ESocketType::Local,
	// 	    .code = code,
	// 	    .clientSlot = client.GetClientSlot(),
	// 	    .serverSlot = server.GetSessionSlot(),
	// 	};
	// 	client.GetEventManager().GetCoreEvents().ClientDisconnect().Invoke(&data, {}, EEventInvocation::None);
	// }

	
}
