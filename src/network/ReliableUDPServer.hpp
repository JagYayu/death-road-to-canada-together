#pragma once

#include "Network.hpp"
#include "Server.hpp"
#include "util/UnorderedBimap.hpp"

struct _ENetHost;
struct _ENetPeer;

namespace tudov
{
	class ReliableUDPServer : public IServer
	{
	  protected:
		INetwork &_network;
		_ENetHost *_eNetHost;
		_ENetPeer *_eNetPeer;
		ClientID _nextClientID;
		UnorderedBimap<std::uint64_t, _ENetPeer *> _clientPeerBimap;

	  public:
		explicit ReliableUDPServer(INetwork &network) noexcept;
		~ReliableUDPServer() noexcept override;

	  protected:
		// 实现根据clientID查找对应ENetPeer的逻辑
		// 通常需要维护一个clientID到ENetPeer*的映射表
		_ENetPeer *GetPeerByClientID(std::uint64_t clientID) noexcept;
		ClientID GetClientIDByPeer(_ENetPeer *peer) noexcept;
		ClientID NewClientID() noexcept;
		void Send(std::uint64_t clientID, std::string_view data, bool reliable);
		void Broadcast(std::string_view data, bool reliable);

	  public:
		ESocketType GetSocketType() const noexcept override;
		INetwork &GetNetwork() noexcept override;
		bool Update() noexcept override;

		// void ProvideDebug(IDebugManager &debugManager) noexcept override;

		void Host(const HostArgs &args) override;
		void Shutdown() override;
		bool IsHosting() noexcept override;
		std::optional<std::string_view> GetTitle() noexcept override;
		std::optional<std::string_view> GetPassword() noexcept override;
		std::optional<std::size_t> GetMaxClients() noexcept override;
		void SendReliable(std::uint64_t clientID, std::string_view data) override;
		void SendUnreliable(std::uint64_t clientID, std::string_view data) override;
		void BroadcastReliable(std::string_view data) override;
		void BroadcastUnreliable(std::string_view data) override;
	};
} // namespace tudov
