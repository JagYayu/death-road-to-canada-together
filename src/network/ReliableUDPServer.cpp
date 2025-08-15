#include "network/ReliableUDPServer.hpp"
#include "event/CoreEvents.hpp"
#include "event/EventManager.hpp"
#include "network/SocketType.hpp"


#include "enet/enet.h"

using namespace tudov;

ReliableUDPServer::ReliableUDPServer(INetworkManager &network) noexcept
    : _networkManager(network),
      _nextClientID(0)
{
}

ReliableUDPServer::~ReliableUDPServer() noexcept
{
	if (IsHosting())
	{
		try
		{
			Shutdown();
		}
		catch (std::exception &e)
		{
			Log::Get("ReliableUDPServer")->Error("Failed to deconstruct: {}", e.what());
		}
	}
}

INetworkManager &ReliableUDPServer::GetNetworkManager() noexcept
{
	return _networkManager;
}

ESocketType ReliableUDPServer::GetSocketType() const noexcept
{
	return ESocketType::RUDP;
}

void ReliableUDPServer::Host(const HostArgs &args)
{
}

void ReliableUDPServer::Shutdown()
{
}

bool ReliableUDPServer::IsHosting() noexcept
{
	return false;
}

std::optional<std::string_view> ReliableUDPServer::GetTitle() noexcept
{
	return std::nullopt;
}

std::optional<std::string_view> ReliableUDPServer::GetPassword() noexcept
{
	return std::nullopt;
}

std::optional<std::size_t> ReliableUDPServer::GetMaxClients() noexcept
{
	return std::nullopt;
}

ReliableUDPServer::ClientID ReliableUDPServer::NewClientID() noexcept
{
	auto clientID = _nextClientID;
	++_nextClientID;
	return clientID;
}

bool ReliableUDPServer::Update() noexcept
{
	ENetEvent event;
	bool update = false;

	while (enet_host_service(_eNetHost, &event, 0) > 0)
	{
		update = true;

		IScriptEngine &scriptEngine = GetScriptEngine();
		ICoreEvents &coreEvents = GetEventManager().GetCoreEvents();

		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
		{
			coreEvents.ClientConnect().Invoke(scriptEngine, nullptr, ESocketType::RUDP);

			// printf("A new client connected from %x:%u.\n",
			//        event.peer->address.host,
			//        event.peer->address.port);

			// /* 存储任何客户端信息 */
			// event.peer->data = "Client information";

			// 可以在这里立即发送欢迎消息
			// const char *welcomeMsg = "Welcome to the server!";
			// ENetPacket *packet = enet_packet_create(welcomeMsg,
			//                                         strlen(welcomeMsg) + 1,
			//                                         ENET_PACKET_FLAG_RELIABLE);
			// enet_peer_send(event.peer, 0, packet);
			break;
		}
		case ENET_EVENT_TYPE_RECEIVE:
		{
			// printf("A packet of length %zu containing '%s' was received from %s on channel %u.\n",
			//        event.packet->dataLength,
			//        (char *)event.packet->data,
			//        (char *)event.peer->data,
			//        event.channelID);

			// // 处理接收到的数据
			// processPacket(event.peer, event.packet);

			// /* 清理数据包 */
			// enet_packet_destroy(event.packet);
			break;
		}
		case ENET_EVENT_TYPE_DISCONNECT:
		{
			// printf("%s disconnected.\n", (char *)event.peer->data);
			// /* 重置客户端信息 */
			// event.peer->data = NULL;
			break;
		}

		case ENET_EVENT_TYPE_NONE:
			break;
		}
	}

	return update;
}

_ENetPeer *ReliableUDPServer::GetPeerByClientID(std::uint64_t clientID) noexcept
{
	return _clientPeerBimap.AtKey(clientID);
}

ReliableUDPServer::ClientID ReliableUDPServer::GetClientIDByPeer(_ENetPeer *peer) noexcept
{
	return _clientPeerBimap.AtValue(peer);
}

void ReliableUDPServer::Send(std::uint64_t clientID, std::string_view data, bool reliable)
{
	if (auto peer = GetPeerByClientID(clientID); peer != nullptr)
	{
		ENetPacket *packet = enet_packet_create(data.data(), data.size(), reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
		if (packet == nullptr || enet_peer_send(peer, 0, packet) != 0)
		{
			enet_packet_destroy(packet);
		}
	}
}

void ReliableUDPServer::SendReliable(std::uint64_t clientID, std::string_view data)
{
	Send(clientID, data, true);
}

void ReliableUDPServer::SendUnreliable(std::uint64_t clientID, std::string_view data)
{
	Send(clientID, data, false);
}

void ReliableUDPServer::Broadcast(std::string_view data, bool reliable)
{
	if (!_clientPeerBimap.Empty())
	{
		ENetPacket *packet = enet_packet_create(data.data(), data.size(), reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
		if (packet == nullptr)
		{
			enet_packet_destroy(packet);
		}

		enet_host_broadcast(_eNetHost, 0, packet);
	}
}

void ReliableUDPServer::BroadcastReliable(std::string_view data)
{
	Broadcast(data, true);
}

void ReliableUDPServer::BroadcastUnreliable(std::string_view data)
{
	Broadcast(data, false);
}

// void ReliableUDPServer::ProvideDebug(IDebugManager &debugManager) noexcept
// {
// 	auto &&debugConsole = debugManager.GetElement<DebugConsole>();
// }
