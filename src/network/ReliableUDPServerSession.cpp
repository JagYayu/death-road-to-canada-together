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

#include "event/CoreEvents.hpp"
#include "event/CoreEventsData.hpp"
#include "event/EventManager.hpp"
#include "event/RuntimeEvent.hpp"
#include "mod/ScriptEngine.hpp"
#include "network/DisconnectionCode.hpp"
#include "network/NetworkSessionData.hpp"
#include "network/ReliableUDPSession.hpp"
#include "network/ServerSessionState.hpp"
#include "network/SocketType.hpp"
#include "program/Tudov.hpp"
#include "system/LogMicros.hpp"

#include "enet/enet.h"
#include "util/Definitions.hpp"

#include <span>
#include <stdexcept>
#include <string>
#include <type_traits>

using namespace tudov;

ReliableUDPServerSession::ReliableUDPServerSession(INetworkManager &network, NetworkSessionSlot serverSlot) noexcept
    : _networkManager(network),
      _serverSessionSlot(serverSlot),
      _eNetHost(nullptr),
      _nextClientSessionID(1)
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

NetworkSessionSlot ReliableUDPServerSession::GetSessionSlot() noexcept
{
	return _serverSessionSlot;
}

Log &ReliableUDPServerSession::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(ReliableUDPServerSession));
}

EServerSessionState ReliableUDPServerSession::GetSessionState() const noexcept
{
	if (_eNetHost != nullptr)
	{
		return EServerSessionState::Hosting;
	}
	return EServerSessionState::Shutdown;
}

std::size_t ReliableUDPServerSession::GetClients() const noexcept
{
	return _clientIDPeerBimap.Size();
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
	if (_eNetHost == nullptr) [[unlikely]]
	{
		throw std::runtime_error("Failed to create enet host");
	}

	char hostName[45];
	enet_address_get_host(&enetAddress, hostName, sizeof(hostName));

	TE_DEBUG("Hosting RUDP server! host: {}, port: {}, slot: {}", hostName, _eNetHost->address.port, _serverSessionSlot);
}

void ReliableUDPServerSession::HostAsync(const IServerSession::HostArgs &args, const ServerHostErrorHandler &handler) noexcept
{
	// TODO
}

void ReliableUDPServerSession::Shutdown()
{
	enet_host_destroy(_eNetHost);
	_eNetHost = nullptr;
}

bool ReliableUDPServerSession::TryShutdown()
{
	if (_eNetHost != nullptr)
	{
		Shutdown();
		return true;
	}
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

ClientSessionID ReliableUDPServerSession::NewClientSessionID() noexcept
{
	auto clientID = _nextClientSessionID;
	++_nextClientSessionID;
	return clientID;
}

bool ReliableUDPServerSession::Update() noexcept
{
	if (_eNetHost == nullptr)
	{
		return false;
	}

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

			ClientSessionID clientID = _nextClientSessionID;
			++_nextClientSessionID;
			_clientIDPeerBimap[clientID] = event.peer;

			EventReliableUDPServerConnectData eventData{
			    .socketType = ESocketType::RUDP,
			    .clientID = clientID,
			    .host = std::string_view(hostName.data()),
			    .port = event.peer->address.port,
			};

			TE_TRACE("Connect event, host: {}, port: {}", eventData.host, eventData.port);

			{ // Send connected client's session id
				std::string data{
				    '\0',
				    static_cast<char>(clientID),
				};

				ENetPacket *packet = enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(event.peer, 0, packet);
			}

			coreEvents.ServerConnect().Invoke(&eventData, _serverSessionSlot, EEventInvocation::None);

			break;
		}
		case ENET_EVENT_TYPE_DISCONNECT:
		{
			std::array<char, 45> hostName;
			enet_address_get_host(&event.peer->address, hostName.data(), hostName.size());

			EventReliableUDPServerDisconnectData eventData{
			    .socketType = ESocketType::RUDP,
			    .clientID = _clientIDPeerBimap.AtValue(event.peer),
			    .host = std::string_view(hostName.data()),
			    .port = event.peer->address.port,
			};

			TE_TRACE("Disconnect event, host: {}, port: {}", eventData.host, eventData.port);

			coreEvents.ServerDisconnect().Invoke(&eventData, _serverSessionSlot, EEventInvocation::None);

			TE_ASSERT(_clientIDPeerBimap.EraseByValue(event.peer));

			break;
		}
		case ENET_EVENT_TYPE_RECEIVE:
			UpdateENetReceive(event);
			break;
		case ENET_EVENT_TYPE_NONE:
			break;
		default:
			break;
		}
	}

	enet_host_flush(_eNetHost);

	return hasEvent;
}

void ReliableUDPServerSession::UpdateENetReceive(_ENetEvent &event) noexcept
{
	if (event.packet->dataLength == 0) [[unlikely]]
	{
		TE_WARN("Received 0 length data");
		return;
	}

	std::array<char, 45> hostName;
	enet_address_get_host(&event.peer->address, hostName.data(), hostName.size());

	EventReliableUDPServerMessageData eventData{
	    .socketType = ESocketType::RUDP,
	    .clientID = _clientIDPeerBimap.AtValue(event.peer),
	    .message = std::string_view(reinterpret_cast<const char *>(event.packet->data), event.packet->dataLength),
	    .broadcast = "",
	    .host = std::string_view(hostName.data()),
	    .port = event.peer->address.port,
	};

	// TE_TRACE("Received event, host: {}, port: {}", eventData.host, eventData.port, eventData.message);

	GetEventManager().GetCoreEvents().ServerMessage().Invoke(&eventData, _serverSessionSlot, EEventInvocation::None);

	if (eventData.broadcast != "")
	{
		NetworkSessionData data;
		data.bytes = std::span<const std::byte>(reinterpret_cast<const std::byte *>(eventData.broadcast.data()), eventData.broadcast.size());
		data.channelID = event.channelID;
		BroadcastReliable(data);
	}
}

_ENetPeer *ReliableUDPServerSession::GetPeerByID(ClientSessionID clientSessionID) noexcept
{
	_ENetPeer **peer = _clientIDPeerBimap.FindByKey(clientSessionID);
	return peer != nullptr ? *peer : nullptr;
}

ClientSessionID ReliableUDPServerSession::GetIDByPeer(_ENetPeer *peer) noexcept
{
	ClientSessionID *id = _clientIDPeerBimap.FindByValue(peer);
	return id != nullptr ? *id : 0;
}

void ReliableUDPServerSession::Disconnect(ClientSessionID clientID, EDisconnectionCode code) noexcept
{
	_ENetPeer *peer = GetPeerByID(clientID);
	if (peer != nullptr)
	{
		TE_TRACE("Disconnect client ID: {}, code: {}", clientID, static_cast<std::underlying_type_t<EDisconnectionCode>>(code));

		enet_peer_disconnect(peer, static_cast<std::uint32_t>(code));
	}
}

void ReliableUDPServerSession::Send(ClientSessionID clientSessionID, const NetworkSessionData &data, bool reliable)
{
	if (_ENetPeer *peer = GetPeerByID(clientSessionID); peer != nullptr)
	{
		ENetPacket *packet = enet_packet_create(data.bytes.data(), data.bytes.size(), reliable ? ENET_PACKET_FLAG_RELIABLE : 0);

		// TE_TRACE("Send message");
		if (enet_peer_send(peer, static_cast<std::uint8_t>(data.channelID), packet) != 0)
		{
			enet_packet_destroy(packet);
		}
	}
}

void ReliableUDPServerSession::SendReliable(ClientSessionID clientSessionID, const NetworkSessionData &data)
{
	Send(clientSessionID, data, true);
}

void ReliableUDPServerSession::SendUnreliable(ClientSessionID clientSessionID, const NetworkSessionData &data)
{
	Send(clientSessionID, data, false);
}

void ReliableUDPServerSession::Broadcast(const NetworkSessionData &data, bool reliable)
{
	if (!_clientIDPeerBimap.Empty())
	{
		ENetPacket *packet = enet_packet_create(data.bytes.data(), data.bytes.size(), reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
		if (packet == nullptr)
		{
			enet_packet_destroy(packet);
		}

		// TE_TRACE("Broadcast message");

		enet_host_broadcast(_eNetHost, data.channelID, packet);
	}
}

void ReliableUDPServerSession::BroadcastReliable(const NetworkSessionData &data)
{
	Broadcast(data, true);
}

void ReliableUDPServerSession::BroadcastUnreliable(const NetworkSessionData &data)
{
	Broadcast(data, false);
}
