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
struct _ENetEvent;

namespace tudov
{
	struct NetworkSessionData;
	struct ReliableUDPSessionData;

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
		NetworkSessionSlot _serverSessionSlot;
		NetworkSessionSlot _serverSlot;
		_ENetHost *_eNetHost;
		ClientSessionID _nextClientSessionID;
		UnorderedBimap<ClientSessionID, _ENetPeer *> _clientIDPeerBimap;

	  public:
		explicit ReliableUDPServerSession(INetworkManager &network, NetworkSessionSlot serverSlot) noexcept;
		~ReliableUDPServerSession() noexcept override;

	  protected:
		_ENetPeer *GetPeerByID(std::uint64_t clientID) noexcept;
		ClientSessionID GetIDByPeer(_ENetPeer *peer) noexcept;
		ClientSessionID NewClientSessionID() noexcept;
		void Send(std::uint64_t clientID, const NetworkSessionData &data, bool reliable);
		void Broadcast(const NetworkSessionData &data, bool reliable);

	  public:
		ESocketType GetSocketType() const noexcept override;
		NetworkSessionSlot GetSessionSlot() noexcept override;
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
		void SendReliable(std::uint64_t clientID, const NetworkSessionData &data) override;
		void SendUnreliable(std::uint64_t clientID, const NetworkSessionData &data) override;
		void BroadcastReliable(const NetworkSessionData &data) override;
		void BroadcastUnreliable(const NetworkSessionData &data) override;

	  private:
		void UpdateENetReceive(_ENetEvent &event) noexcept;
	};
} // namespace tudov
