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

#include "Network/LocalSession.hpp"

#include "Event/CoreEvents.hpp"
#include "Event/CoreEventsData.hpp"
#include "Event/EventInvocation.hpp"
#include "Event/EventManager.hpp"
#include "Event/RuntimeEvent.hpp"
#include "Network/DisconnectionCode.hpp"
#include "Network/LocalClientSession.hpp"
#include "Network/LocalServerSession.hpp"
#include "Network/LocalSession.hpp"
#include "Network/NetworkManager.hpp"
#include "Network/SocketType.hpp"

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
