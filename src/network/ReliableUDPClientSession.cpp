/**
 * @file network/ReliableUDPClient.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "network/ReliableUDPClientSession.hpp"

#include "data/Constants.hpp"
#include "event/CoreEvents.hpp"
#include "event/CoreEventsData.hpp"
#include "event/EventManager.hpp"
#include "event/RuntimeEvent.hpp"
#include "network/ClientSessionState.hpp"
#include "network/DisconnectionCode.hpp"
#include "network/NetworkManager.hpp"
#include "network/NetworkSessionData.hpp"
#include "network/ReliableUDPSession.hpp"
#include "network/SocketType.hpp"
#include "system/LogMicros.hpp"

#include "enet/enet.h"

#include <array>
#include <stdexcept>
#include <type_traits>

using namespace tudov;

ReliableUDPClientSession::ReliableUDPClientSession(INetworkManager &networkManager, NetworkSessionSlot clientSlot) noexcept
    : _networkManager(networkManager),
      _clientSessionSlot(clientSlot),
      _clientSessionID(0),
      _eNetHost(nullptr),
      _eNetPeer(nullptr)
{
	ReliableUDPSession::OnENetSessionInitialize();
}

ReliableUDPClientSession::~ReliableUDPClientSession() noexcept
{
	TryDisconnect(EDisconnectionCode::ClientClosed);

	ReliableUDPSession::OnENetSessionDeinitialize();
}

Context &ReliableUDPClientSession::GetContext() noexcept
{
	return _networkManager.GetContext();
}

Log &ReliableUDPClientSession::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(ReliableUDPClientSession));
}

INetworkManager &ReliableUDPClientSession::GetNetworkManager() noexcept
{
	return _networkManager;
}

NetworkSessionSlot ReliableUDPClientSession::GetSessionSlot() noexcept
{
	return _clientSessionSlot;
}

ESocketType ReliableUDPClientSession::GetSocketType() const noexcept
{
	return ESocketType::RUDP;
}

ClientSessionID ReliableUDPClientSession::GetSessionID() const noexcept
{
	return _clientSessionID;
}

EClientSessionState ReliableUDPClientSession::GetSessionState() const noexcept
{
	return EClientSessionState::Disconnected;
}

void ReliableUDPClientSession::TryCreateENetHost()
{
	if (_eNetHost == nullptr)
	{
		_eNetHost = enet_host_create(nullptr, 1, NetworkChannelsLimit, 0, 0);
		if (_eNetHost == nullptr)
		{
			throw std::runtime_error("Failed to create ENet client host");
		}
	}
}

void ReliableUDPClientSession::Connect(const IClientSession::ConnectArgs &baseArgs)
{
	if (_eNetPeer && _eNetPeer->state != ENET_PEER_STATE_DISCONNECTED)
	{
		throw std::runtime_error("Client is already connected or connecting");
	}

	auto &&args = dynamic_cast<const ConnectArgs &>(baseArgs);

	ENetAddress enetAddress;
	if (enet_address_set_host(&enetAddress, args.host.data()) != 0) [[unlikely]]
	{
		throw std::runtime_error("Failed to resolve hostname");
	}
	enetAddress.port = args.port;

	TryCreateENetHost();

	_eNetPeer = enet_host_connect(_eNetHost, &enetAddress, NetworkChannelsLimit, 0);
	if (_eNetPeer == nullptr) [[unlikely]]
	{
		throw std::runtime_error("Failed to create peer for connection");
	}

	TE_DEBUG("Connecting to server");

	// _ENetEvent event;
	// if (enet_host_service(_eNetHost, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	// {
	// 	TE_DEBUG("Connected to server!");
	// }
	// else
	// {
	// 	enet_peer_reset(_eNetPeer);
	// 	_eNetPeer = nullptr;
	// 	TE_DEBUG("Connection failed");
	// }
}

void ReliableUDPClientSession::Disconnect(EDisconnectionCode code)
{
	enet_peer_disconnect_now(_eNetPeer, static_cast<std::uint32_t>(code));
	enet_peer_reset(_eNetPeer);
	enet_host_destroy(_eNetHost);

	_eNetPeer = nullptr;
	_eNetHost = nullptr;
	_clientSessionID = 0;

	while (Update())
	{
	}
}

bool ReliableUDPClientSession::TryDisconnect(EDisconnectionCode code)
{
	if (GetSessionState() == EClientSessionState::Disconnected)
	{
		return false;
	}

	Disconnect(code);
	return true;
}

TE_FORCEINLINE void Send(ENetPeer *peer, const NetworkSessionData &data, enet_uint32 flags)
{
	ENetPacket *packet = enet_packet_create(data.bytes.data(), data.bytes.size(), flags);
	enet_peer_send(peer, static_cast<std::uint8_t>(data.channelID), packet);
}

void ReliableUDPClientSession::SendReliable(const NetworkSessionData &data)
{
	// TE_TRACE("Send message");
	Send(_eNetPeer, data, ENET_PACKET_FLAG_RELIABLE);
}

void ReliableUDPClientSession::SendUnreliable(const NetworkSessionData &data)
{
	// TE_TRACE("Send message");
	Send(_eNetPeer, data, 0);
}

bool ReliableUDPClientSession::Update()
{
	if (_eNetHost == nullptr)
	{
		return false;
	}

	ICoreEvents &coreEvents = GetEventManager().GetCoreEvents();

	ENetEvent event;
	bool hasEvent = false;

	while (enet_host_service(_eNetHost, &event, 0) != 0)
	{
		hasEvent = true;

		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
		{
			std::array<char, 45> hostName;
			enet_address_get_host(&event.peer->address, hostName.data(), hostName.size());

			EventReliableUDPClientConnectData data{
			    .host = std::string_view(hostName.data()),
			    .port = event.peer->address.port,
			};

			TE_TRACE("Connected to server! event, host: {}, port: {}", data.host, data.port);

			coreEvents.ClientConnect().Invoke(&data, EventHandleKey(_clientSessionSlot), EEventInvocation::None);

			break;
		}
		case ENET_EVENT_TYPE_DISCONNECT:
		{
			std::array<char, 45> hostName;
			enet_address_get_host(&event.peer->address, hostName.data(), hostName.size());

			EventReliableUDPClientDisconnectData eventData{
			    .socketType = ESocketType::RUDP,
			    .clientID = _clientSessionID,
			    // .code TODO what is the code?
			    .host = std::string_view(hostName.data()),
			    .port = event.peer->address.port,
			};

			TE_TRACE("Disconnected from server! event, host: {}, port: {}", eventData.host, eventData.port);

			coreEvents.ClientMessage().Invoke(&eventData, EventHandleKey(_clientSessionSlot), EEventInvocation::None);

			_clientSessionID = 0;

			break;
		}
		case ENET_EVENT_TYPE_RECEIVE:
			UpdateENetReceive(event);
			break;
		case ENET_EVENT_TYPE_NONE:
			break;
		default:
			TE_WARN("Unknown event type {}", static_cast<std::underlying_type_t<ENetEventType>>(event.type));
			break;
		}
	}

	enet_host_flush(_eNetHost);

	return hasEvent;
}

void ReliableUDPClientSession::UpdateENetReceive(_ENetEvent &event) noexcept
{
	std::array<char, 45> hostName;
	enet_address_get_host(&event.peer->address, hostName.data(), hostName.size());

	std::string_view received{reinterpret_cast<const char *>(event.packet->data), event.packet->dataLength};

	EventReliableUDPClientMessageData data{
	    .socketType = ESocketType::RUDP,
	    .message = received,
	    .host = std::string_view(hostName.data()),
	    .port = event.peer->address.port,
	};

	// TE_TRACE("Received event, host: {}, port: {}, message: {}B", data.host, data.port, data.message.size());

	if (received.size() == 2 && received[0] == '\0') [[unlikely]]
	{
		// Special case: receive client's session id
		_clientSessionID = static_cast<ClientSessionID>(received[1]);

		TE_TRACE("Received client session id {} from server", _clientSessionID);
	}
	else
	{
		GetEventManager().GetCoreEvents().ClientMessage().Invoke(&data, EventHandleKey(_clientSessionSlot), EEventInvocation::None);
	}

	enet_packet_destroy(event.packet);
}
