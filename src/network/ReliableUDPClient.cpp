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

#include "network/ReliableUDPClient.hpp"

#include "data/Constants.hpp"
#include "event/CoreEvents.hpp"
#include "event/EventManager.hpp"
#include "event/RuntimeEvent.hpp"
#include "network/NetworkManager.hpp"

#include "enet/enet.h"

#include <stdexcept>

using namespace tudov;

static std::uint32_t enetRefCount = 0;

ReliableUDPClient::ReliableUDPClient(INetworkManager &networkManager) noexcept
    : _networkManager(networkManager)
{
	if (enetRefCount == 0)
	{
		enet_initialize();
	}
	++enetRefCount;
}

ReliableUDPClient::~ReliableUDPClient() noexcept
{
	--enetRefCount;
	if (enetRefCount == 0)
	{
		enet_deinitialize();
	}
}

Context &ReliableUDPClient::GetContext() noexcept
{
	return _networkManager.GetContext();
}

INetworkManager &ReliableUDPClient::GetNetworkManager() noexcept
{
	return _networkManager;
}

ESocketType ReliableUDPClient::GetSocketType() const noexcept
{
	return ESocketType::RUDP;
}

bool ReliableUDPClient::IsConnecting() const noexcept
{
	return _isConnecting;
}

bool ReliableUDPClient::IsConnected() const noexcept
{
	return !_isConnecting && _eNetPeer != nullptr;
}

void ReliableUDPClient::TryCreateENetHost()
{
	if (_eNetHost)
	{
		return;
	}

	_eNetHost = enet_host_create(nullptr, 1, NetworkChannelsLimit, 0, 0);
	if (_eNetHost == nullptr)
	{
		throw std::runtime_error("Failed to create ENet client host");
	}
}

void ReliableUDPClient::Connect(const IClient::ConnectArgs &baseArgs)
{
	auto &&args = dynamic_cast<const ConnectArgs &>(baseArgs);

	TryCreateENetHost();

	// size_t colonPos = address.find(':');
	// if (colonPos == std::string_view::npos)
	// {
	// 	throw std::runtime_error("Invalid address format, expected \"<hostname>:<port>\"");
	// }

	// std::string hostname{address.substr(0, colonPos)};
	// std::int32_t port = std::stoi(std::string(address.substr(colonPos + 1)));

	ENetAddress enetAddress;
	if (enet_address_set_host(&enetAddress, args.host.data()) != 0)
	{
		throw std::runtime_error("Failed to resolve hostname");
	}
	enetAddress.port = args.port;

	_eNetPeer = enet_host_connect(_eNetHost, &enetAddress, NetworkChannelsLimit, 0);
	if (_eNetPeer == nullptr)
	{
		throw std::runtime_error("Failed to create peer for connection");
	}

	_isConnecting = true;
}

void ReliableUDPClient::Disconnect()
{
}

TE_FORCEINLINE void Send(ENetPeer *peer, std::string_view data, enet_uint32 flags)
{
	ENetPacket *packet = enet_packet_create(data.data(), data.length(), flags);
	enet_peer_send(peer, 0, packet);
	enet_packet_destroy(packet);
}

void ReliableUDPClient::SendReliable(std::string_view data)
{
	Send(_eNetPeer, data, ENET_PACKET_FLAG_RELIABLE);
}

void ReliableUDPClient::SendUnreliable(std::string_view data)
{
	Send(_eNetPeer, data, 0);
}

bool ReliableUDPClient::Update()
{
	if (!_eNetHost)
	{
		return false;
	}

	enet_host_flush(_eNetHost);

	auto &&coreEvents = GetEventManager().GetCoreEvents();

	bool hasEvent = false;
	ENetEvent event;
	while (enet_host_service(_eNetHost, &event, 0) > 0)
	{
		hasEvent = true;

		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
		{
			if (_isConnecting)
			{
				_isConnecting = false;

				coreEvents.ClientConnect().Invoke();
			}
			break;
		}
		case ENET_EVENT_TYPE_RECEIVE:
			// OnDataReceived(event.packet->data, event.packet->dataLength);
			// enet_packet_destroy(event.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			// isConnecting = false;
			// isConnected = false;
			// OnDisconnected();
			break;
		default:
			break;
		}
	}

	return hasEvent;
}
