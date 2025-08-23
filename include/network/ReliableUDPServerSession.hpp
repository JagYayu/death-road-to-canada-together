/**
 * @file network/ReliableUDPServer.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "NetworkManager.hpp"
#include "ServerSession.hpp"
#include "system/Log.hpp"
#include "util/UnorderedBimap.hpp"

#include <cstdint>

struct _ENetHost;
struct _ENetPeer;

namespace tudov
{
	class ReliableUDPServerSession : public IServerSession, private ILogProvider
	{
	  public:
		struct HostArgs : IServerSession::HostArgs
		{
			~HostArgs() noexcept override = default;

			std::string_view host;
			std::uint16_t port;
		};

	  protected:
		INetworkManager &_networkManager;
		_ENetHost *_eNetHost;
		ClientSessionToken _nextClientSessionToken;
		UnorderedBimap<ClientSessionToken, _ENetPeer *> _clientTokenPeerBimap;

	  public:
		explicit ReliableUDPServerSession(INetworkManager &network) noexcept;
		~ReliableUDPServerSession() noexcept override;

	  protected:
		_ENetPeer *GetPeerByToken(std::uint64_t clientID) noexcept;
		ClientSessionToken GetTokenByPeer(_ENetPeer *peer) noexcept;
		ClientSessionToken NewToken() noexcept;
		void Send(std::uint64_t clientID, std::span<const std::byte> data, ChannelID channelID, bool reliable);
		void Broadcast(std::span<const std::byte> data, ChannelID channelID, bool reliable);

	  public:
		ESocketType GetSocketType() const noexcept override;
		Log &GetLog() noexcept override;
		INetworkManager &GetNetworkManager() noexcept override;
		bool Update() noexcept override;

		EServerSessionState GetSessionState() const noexcept override;
		void Host(const IServerSession::HostArgs &args) override;
		void Shutdown() override;
		bool TryShutdown() override;
		std::optional<std::string_view> GetTitle() const noexcept override;
		std::optional<std::string_view> GetPassword() const noexcept override;
		std::optional<std::size_t> GetMaxClients() const noexcept override;
		void SendReliable(std::uint64_t clientID, std::span<const std::byte> data, ChannelID channelID) override;
		void SendUnreliable(std::uint64_t clientID, std::span<const std::byte> data, ChannelID channelID) override;
		void BroadcastReliable(std::span<const std::byte> data, ChannelID channelID) override;
		void BroadcastUnreliable(std::span<const std::byte> data, ChannelID channelID) override;
	};
} // namespace tudov
