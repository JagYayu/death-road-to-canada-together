/**
 * @file network/LocalServerSession.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "LocalSession.hpp"
#include "ServerSession.hpp"
#include "SocketType.hpp"
#include "data/Constants.hpp"
#include "system/Log.hpp"
#include "util/Definitions.hpp"

#include <cstdint>
#include <memory>
#include <queue>
#include <unordered_map>

namespace tudov
{
	struct INetworkManager;
	struct INetworkManager;
	struct NetworkSessionData;
	class LocalClientSession;

	class LocalServerSession : public IServerSession, private ILogProvider
	{
		friend LocalClientSession;

	  public:
		using HostArgs = IServerSession::HostArgs;

	  protected:
		struct HostInfo
		{
			std::string_view title = NetworkServerTitle;
			std::string_view password = NetworkServerPassword;
			std::size_t maximumClients = NetworkServerMaximumClients;
			std::unordered_map<ClientSessionID, std::weak_ptr<LocalClientSession>> localClients;
		};

	  protected:
		INetworkManager &_networkManager;
		EServerSessionState _sessionState;
		NetworkSessionSlot _serverSessionSlot;
		std::unique_ptr<HostInfo> _hostInfo;
		std::queue<LocalSessionMessage> _messageQueue;
		ClientSessionID _latestID;

	  public:
		explicit LocalServerSession(INetworkManager &networkManager, NetworkSessionSlot serverSlot) noexcept;
		~LocalServerSession() noexcept override = default;

	  public:
		INetworkManager &GetNetworkManager() noexcept override;
		NetworkSessionSlot GetSessionSlot() noexcept override;
		ESocketType GetSocketType() const noexcept override;
		Log &GetLog() noexcept override;

		EServerSessionState GetSessionState() const noexcept override;
		void Host(const HostArgs &args) override;
		void Shutdown() override;
		bool TryShutdown() override;
		std::optional<std::string_view> GetTitle() const noexcept override;
		std::optional<std::string_view> GetPassword() const noexcept override;
		std::optional<std::size_t> GetMaxClients() const noexcept override;
		void Disconnect(ClientSessionID clientSessionID, EDisconnectionCode code) override;
		void SendReliable(ClientSessionID clientSessionID, const NetworkSessionData &data) override;
		void SendUnreliable(ClientSessionID clientSessionID, const NetworkSessionData &data) override;
		void BroadcastReliable(const NetworkSessionData &data) override;
		void BroadcastUnreliable(const NetworkSessionData &data) override;

		bool Update() override;

		ClientSessionID ClientSlotToClientID(NetworkSessionSlot clientSlot) const noexcept;

		void Receive(const LocalSessionMessage &message) noexcept;

		void AddClient(NetworkSessionSlot clientSlot, std::weak_ptr<LocalClientSession> localClient);

	  protected:
		ClientSessionID NewID() noexcept;
		void EnqueueMessage(ClientSessionID clientSessionID, const NetworkSessionData &data, ELocalSessionSource source);
	};
} // namespace tudov
