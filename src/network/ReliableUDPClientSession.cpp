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
#include "network/NetworkManager.hpp"

#include "enet/enet.h"
#include "network/ReliableUDPSession.hpp"
#include "network/SocketType.hpp"
#include "system/LogMicros.hpp"

#include <array>
#include <stdexcept>
#include <type_traits>

using namespace tudov;

ReliableUDPClientSession::ReliableUDPClientSession(INetworkManager &networkManager) noexcept
    : _networkManager(networkManager),
      _clientToken(0),
      _eNetHost(nullptr),
      _eNetPeer(nullptr)
{
	ReliableUDPSession::OnENetSessionInitialize();
}

ReliableUDPClientSession::~ReliableUDPClientSession() noexcept
{
	TryDisconnect();

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

ESocketType ReliableUDPClientSession::GetSocketType() const noexcept
{
	return ESocketType::RUDP;
}

ClientSessionToken ReliableUDPClientSession::GetToken() const noexcept
{
	return _clientToken;
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

	auto &args = dynamic_cast<const ConnectArgs &>(baseArgs);

	ENetAddress enetAddress;
	if (enet_address_set_host(&enetAddress, args.host.data()) != 0)
	{
		throw std::runtime_error("Failed to resolve hostname");
	}
	enetAddress.port = args.port;

	TryCreateENetHost();

	_eNetPeer = enet_host_connect(_eNetHost, &enetAddress, NetworkChannelsLimit, 0);
	if (_eNetPeer == nullptr)
	{
		throw std::runtime_error("Failed to create peer for connection");
	}

	TE_DEBUG("Connecting to server");

	_ENetEvent event;
	if (enet_host_service(_eNetHost, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		TE_DEBUG("Connected!");
	}
	else
	{
		enet_peer_reset(_eNetPeer);
		_eNetPeer = nullptr;
		TE_DEBUG("Connection failed");
	}
}

void ReliableUDPClientSession::Disconnect()
{
	enet_peer_disconnect_now(_eNetPeer, 0);
	enet_peer_reset(_eNetPeer);
	enet_host_destroy(_eNetHost);

	_eNetPeer = nullptr;
	_eNetHost = nullptr;
}

bool ReliableUDPClientSession::TryDisconnect()
{
	if (GetSessionState() == EClientSessionState::Disconnected)
	{
		return false;
	}

	Disconnect();
	return true;
}

TE_FORCEINLINE void Send(ENetPeer *peer, std::span<const std::byte> data, ChannelID channelID, enet_uint32 flags)
{
	ENetPacket *packet = enet_packet_create(data.data(), data.size(), flags);
	enet_peer_send(peer, static_cast<std::uint8_t>(channelID), packet);
	enet_packet_destroy(packet);
}

void ReliableUDPClientSession::SendReliable(std::span<const std::byte> data, ChannelID channelID)
{
	TE_TRACE("Send message");
	Send(_eNetPeer, data, channelID, ENET_PACKET_FLAG_RELIABLE);
}

void ReliableUDPClientSession::SendUnreliable(std::span<const std::byte> data, ChannelID channelID)
{
	TE_TRACE("Send message");
	Send(_eNetPeer, data, channelID, 0);
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

			EventReliableUDPClientConnectData e{
			    .host = std::string_view(hostName.data()),
			    .port = event.peer->address.port,
			};

			TE_TRACE("Connect event, host: {}, port: {}", e.host, ENET_NET_TO_HOST_16(e.port));

			coreEvents.ClientConnect().Invoke(&e, {}, EEventInvocation::None);

			break;
		}
		case ENET_EVENT_TYPE_RECEIVE:
		{
			std::array<char, 45> hostName;
			enet_address_get_host(&event.peer->address, hostName.data(), hostName.size());

			std::string_view receivedData{reinterpret_cast<const char *>(event.packet->data), event.packet->dataLength};

			EventReliableUDPClientMessageData data{
			    .socketType = ESocketType::RUDP,
			    .message = receivedData,
			    .host = std::string_view(hostName.data()),
			    .port = event.peer->address.port,
			};

			TE_TRACE("Connect event, host: {}, port: {}, message: {} bytes", data.host, ENET_NET_TO_HOST_16(data.port), data.message.size());

			coreEvents.ClientMessage().Invoke(&data, {}, EEventInvocation::None);

			enet_packet_destroy(event.packet);

			break;
		}
		case ENET_EVENT_TYPE_DISCONNECT:
		{
			std::array<char, 45> hostName;
			enet_address_get_host(&event.peer->address, hostName.data(), hostName.size());

			EventReliableUDPClientDisconnectData data{
			    .socketType = ESocketType::RUDP,
			    .host = std::string_view(hostName.data()),
			    .port = event.peer->address.port,
			};

			TE_TRACE("Disconnect event, host: {}, port: {}", data.host, ENET_NET_TO_HOST_16(data.port));

			coreEvents.ClientMessage().Invoke(&data, {}, EEventInvocation::None);

			enet_packet_destroy(event.packet);

			break;
		}
		default:
			TE_WARN("Unknown event type {}", static_cast<std::underlying_type_t<ENetEventType>>(event.type));
			break;
		}
	}

	enet_host_flush(_eNetHost);

	return hasEvent;
}
