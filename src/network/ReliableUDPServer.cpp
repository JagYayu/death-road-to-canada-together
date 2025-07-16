#include "ReliableUDPServer.hpp"

#include "SocketType.hpp"
#include "debug/DebugConsole.hpp"
#include "debug/DebugManager.hpp"
#include "event/CoreEvents.hpp"
#include "event/EventManager.hpp"

#include "enet/enet.h"

using namespace tudov;

ReliableUDPServer::ReliableUDPServer(INetwork &network) noexcept
    : _network(network)
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

INetwork &ReliableUDPServer::GetNetwork() noexcept
{
	return _network;
}

ESocketType ReliableUDPServer::GetSocketType() const noexcept
{
	return ESocketType::ReliableUDP;
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

std::optional<std::int32_t> ReliableUDPServer::GetMaxClients() noexcept
{
	return std::nullopt;
}

bool ReliableUDPServer::Update() noexcept
{
	ENetEvent event;
	bool update = false;

	while (enet_host_service(_eNetHost, &event, 0) > 0)
	{
		update = true;

		auto &&coreEvents = GetEventManager().GetCoreEvents();

		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
		{
			// coreEvents.ClientMessage();

			// printf("A new client connected from %x:%u.\n",
			//        event.peer->address.host,
			//        event.peer->address.port);

			// /* 存储任何客户端信息 */
			// event.peer->data = "Client information";

			// // 可以在这里立即发送欢迎消息
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

void ReliableUDPServer::ProvideDebug(IDebugManager &debugManager) noexcept
{
	auto &&debugConsole = debugManager.GetElement<DebugConsole>();
}
