/**
 * @file network/ReliableUDPServer.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "network/ReliableUDPServerSession.hpp"

#include "enet/win32.h"
#include "event/CoreEvents.hpp"
#include "event/CoreEventsData.hpp"
#include "event/EventManager.hpp"
#include "event/RuntimeEvent.hpp"
#include "mod/ScriptEngine.hpp"
#include "network/DisconnectionCode.hpp"
#include "network/ReliableUDPSession.hpp"
#include "network/ServerSessionState.hpp"
#include "network/SocketType.hpp"

#include "enet/enet.h"
#include "program/Tudov.hpp"
#include "system/LogMicros.hpp"
#include <span>
#include <stdexcept>
#include <string>

using namespace tudov;

ReliableUDPServerSession::ReliableUDPServerSession(INetworkManager &network) noexcept
    : _networkManager(network),
      _eNetHost(nullptr),
      _nextClientSessionToken(1)
{
	ReliableUDPSession::OnENetSessionInitialize();
}

ReliableUDPServerSession::~ReliableUDPServerSession() noexcept
{
	try
	{
		TryShutdown();
	}
	catch (std::exception &e)
	{
		Log::Get("ReliableUDPServer")->Error("Failed to deconstruct: {}", e.what());
	}

	ReliableUDPSession::OnENetSessionDeinitialize();
}

INetworkManager &ReliableUDPServerSession::GetNetworkManager() noexcept
{
	return _networkManager;
}

ESocketType ReliableUDPServerSession::GetSocketType() const noexcept
{
	return ESocketType::RUDP;
}

Log &ReliableUDPServerSession::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(ReliableUDPServerSession));
}

EServerSessionState ReliableUDPServerSession::GetSessionState() const noexcept
{
	if (_eNetHost)
	{
		return EServerSessionState::Hosting;
	}
	return EServerSessionState::Shutdown;
}

void ReliableUDPServerSession::Host(const IServerSession::HostArgs &baseArgs)
{
	if (GetSessionState() != EServerSessionState::Shutdown)
	{
		throw std::runtime_error("Client is already connected or connecting");
	}

	const auto &args = dynamic_cast<const HostArgs &>(baseArgs);

	ENetAddress enetAddress;
	if (enet_address_set_host(&enetAddress, args.host.data()) != 0)
	{
		throw std::runtime_error("Failed to resolve hostname");
	}
	enetAddress.port = args.port;

	_eNetHost = enet_host_create(&enetAddress, args.maximumClients, NetworkChannelsLimit, 0, 0);
	if (_eNetHost == nullptr)
	{
		throw std::runtime_error("Failed to create enet host");
	}

	char hostName[45];
	enet_address_get_host(&enetAddress, hostName, sizeof(hostName));
	TE_DEBUG("Created host: {}, port: {}", hostName, ENET_NET_TO_HOST_16(_eNetHost->address.port));
}

void ReliableUDPServerSession::Shutdown()
{
}

bool ReliableUDPServerSession::TryShutdown()
{
	return false;
}

std::optional<std::string_view> ReliableUDPServerSession::GetTitle() const noexcept
{
	return std::nullopt;
}

std::optional<std::string_view> ReliableUDPServerSession::GetPassword() const noexcept
{
	return std::nullopt;
}

std::optional<std::size_t> ReliableUDPServerSession::GetMaxClients() const noexcept
{
	return std::nullopt;
}

ClientSessionToken ReliableUDPServerSession::NewToken() noexcept
{
	auto clientID = _nextClientSessionToken;
	++_nextClientSessionToken;
	return clientID;
}

bool ReliableUDPServerSession::Update() noexcept
{
	if (_eNetHost == nullptr)
	{
		return false;
	}

	enet_host_flush(_eNetHost);

	ENetEvent event;
	bool hasEvent = false;

	while (enet_host_service(_eNetHost, &event, 0) != 0)
	{
		hasEvent = true;

		IScriptEngine &scriptEngine = GetScriptEngine();
		ICoreEvents &coreEvents = GetEventManager().GetCoreEvents();

		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
		{
			std::array<char, 45> hostName;
			enet_address_get_host(&event.peer->address, hostName.data(), hostName.size());

			EventReliableUDPServerConnectData data{
			    .socketType = ESocketType::RUDP,
			    .host = std::string_view(hostName.data()),
			    .port = event.peer->address.port,
			    .disconnect = EDisconnectionCode::None,
			};

			TE_TRACE("Connect event, host: {}, port: {}", data.host, ENET_NET_TO_HOST_16(data.port));

			coreEvents.ServerConnect().Invoke(&data, data.socketType, EEventInvocation::None);

			if (data.disconnect != EDisconnectionCode::None)
			{
				enet_peer_disconnect_later(event.peer, static_cast<int>(data.disconnect));
			}
			else
			{
				ClientSessionToken token = NewToken();

				std::string msg = std::to_string(token);

				ENetPacket *packet = enet_packet_create(msg.data(), msg.size(), ENET_PACKET_FLAG_RELIABLE);

				enet_peer_send(event.peer, 0, packet);
			}

			std::span<std::byte> broadcastData{reinterpret_cast<std::byte *>(event.packet->data), event.packet->dataLength};
			BroadcastReliable(broadcastData, event.channelID);

			break;
		}
		case ENET_EVENT_TYPE_RECEIVE:
		{
			std::array<char, 45> hostName;
			enet_address_get_host(&event.peer->address, hostName.data(), hostName.size());

			EventReliableUDPServerMessageData data{
			    .socketType = ESocketType::RUDP,
			    .clientToken = _clientTokenPeerBimap.AtValue(event.peer),
			    .message = std::string_view(reinterpret_cast<char *>(event.packet->data)),
			    .messageOverride = "",
			    .host = std::string_view(hostName.data()),
			    .port = event.peer->address.port,
			};

			TE_TRACE("Receive event, host: {}, port: {}", data.host, ENET_NET_TO_HOST_16(data.port));

			coreEvents.ServerMessage().Invoke(&data, data.socketType, EEventInvocation::None);

			std::span<const std::byte> message;
			if (data.messageOverride != "")
			{
				message = std::span<const std::byte>(reinterpret_cast<const std::byte *>(event.packet->data), event.packet->dataLength);
			}
			else
			{
				message = std::span<const std::byte>(reinterpret_cast<const std::byte *>(data.message.data()), data.message.size());
			}
			BroadcastReliable(message, event.channelID);

			break;
		}
		case ENET_EVENT_TYPE_DISCONNECT:
		{
			std::array<char, 45> hostName;
			enet_address_get_host(&event.peer->address, hostName.data(), hostName.size());

			EventReliableUDPServerDisconnectData data{
			    .socketType = ESocketType::RUDP,
			    .clientToken = _clientTokenPeerBimap.AtValue(event.peer),
			    .host = std::string_view(hostName.data()),
			    .port = event.peer->address.port,
			};

			TE_TRACE("Disconnect event, host: {}, port: {}", data.host, ENET_NET_TO_HOST_16(data.port));

			coreEvents.ServerDisconnect().Invoke(&data, data.socketType, EEventInvocation::None);

			std::span<std::byte> broadcastData{reinterpret_cast<std::byte *>(event.packet->data), event.packet->dataLength};
			BroadcastReliable(broadcastData, event.channelID);

			TE_ASSERT(_clientTokenPeerBimap.EraseByValue(event.peer));

			break;
		}
		case ENET_EVENT_TYPE_NONE:
			break;
		}
	}

	return hasEvent;
}

_ENetPeer *ReliableUDPServerSession::GetPeerByToken(std::uint64_t clientSessionToken) noexcept
{
	return _clientTokenPeerBimap.AtKey(clientSessionToken);
}

ClientSessionToken ReliableUDPServerSession::GetTokenByPeer(_ENetPeer *peer) noexcept
{
	return _clientTokenPeerBimap.AtValue(peer);
}

void ReliableUDPServerSession::Send(std::uint64_t clientSessionToken, std::span<const std::byte> data, ChannelID channelID, bool reliable)
{
	if (_ENetPeer *peer = GetPeerByToken(clientSessionToken); peer != nullptr)
	{
		ENetPacket *packet = enet_packet_create(data.data(), data.size(), reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
		if (packet == nullptr)
		{
			enet_packet_destroy(packet);
		}

		TE_TRACE("Send message");
		if (enet_peer_send(peer, static_cast<std::uint8_t>(channelID), packet) != 0)
		{
			enet_packet_destroy(packet);
		}
	}
}

void ReliableUDPServerSession::SendReliable(std::uint64_t clientSessionToken, std::span<const std::byte> data, ChannelID channelID)
{
	Send(clientSessionToken, data, channelID, true);
}

void ReliableUDPServerSession::SendUnreliable(std::uint64_t clientSessionToken, std::span<const std::byte> data, ChannelID channelID)
{
	Send(clientSessionToken, data, channelID, false);
}

void ReliableUDPServerSession::Broadcast(std::span<const std::byte> data, ChannelID channelID, bool reliable)
{
	if (!_clientTokenPeerBimap.Empty())
	{
		ENetPacket *packet = enet_packet_create(data.data(), data.size(), reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
		if (packet == nullptr)
		{
			enet_packet_destroy(packet);
		}

		TE_TRACE("Broadcast message");
		enet_host_broadcast(_eNetHost, channelID, packet);
	}
}

void ReliableUDPServerSession::BroadcastReliable(std::span<const std::byte> data, ChannelID channelID)
{
	Broadcast(data, channelID, true);
}

void ReliableUDPServerSession::BroadcastUnreliable(std::span<const std::byte> data, ChannelID channelID)
{
	Broadcast(data, channelID, false);
}

// void ReliableUDPServer::ProvideDebug(IDebugManager &debugManager) noexcept
// {
// 	auto &&debugConsole = debugManager.GetElement<DebugConsole>();
// }
