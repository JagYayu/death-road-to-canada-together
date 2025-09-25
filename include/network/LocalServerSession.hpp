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

#include <memory>
#include <queue>
#include <unordered_map>

namespace tudov
{
	enum class EDisconnectionCode : std::uint32_t;
	struct INetworkManager;
	struct INetworkManager;
	struct NetworkSessionData;
	class LocalClientSession;
	class LocalSession;

	class LocalServerSession : public IServerSession, private ILogProvider
	{
		friend LocalClientSession;
		friend LocalSession;

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
		ClientSessionID _latestClientSessionID;

	  public:
		explicit LocalServerSession(INetworkManager &networkManager, NetworkSessionSlot serverSlot) noexcept;
		explicit LocalServerSession(const LocalServerSession &) noexcept = delete;
		explicit LocalServerSession(LocalServerSession &&) noexcept = delete;
		LocalServerSession &operator=(const LocalServerSession &) noexcept = delete;
		LocalServerSession &operator=(LocalServerSession &&) noexcept = delete;
		~LocalServerSession() noexcept override = default;

	  public:
		[[nodiscard]] INetworkManager &GetNetworkManager() noexcept override;
		[[nodiscard]] NetworkSessionSlot GetSessionSlot() noexcept override;
		[[nodiscard]] ESocketType GetSocketType() const noexcept override;
		[[nodiscard]] Log &GetLog() noexcept override;

		[[nodiscard]] EServerSessionState GetSessionState() const noexcept override;
		[[nodiscard]] std::size_t GetClients() const noexcept override;
		void Host(const HostArgs &args) override;
		void HostAsync(const HostArgs &args, const ServerHostErrorHandler &handler) noexcept override;
		void Shutdown() override;
		bool TryShutdown() override;
		[[nodiscard]] std::optional<std::string_view> GetTitle() const noexcept override;
		[[nodiscard]] std::optional<std::string_view> GetPassword() const noexcept override;
		[[nodiscard]] std::optional<std::size_t> GetMaxClients() const noexcept override;
		void Disconnect(ClientSessionID clientSessionID, EDisconnectionCode code) override;
		void SendReliable(ClientSessionID clientSessionID, const NetworkSessionData &data) override;
		void SendUnreliable(ClientSessionID clientSessionID, const NetworkSessionData &data) override;
		void BroadcastReliable(const NetworkSessionData &data) override;
		void BroadcastUnreliable(const NetworkSessionData &data) override;

		bool Update() override;

		[[deprecated]] ClientSessionID ClientSlotToClientID(NetworkSessionSlot clientSlot) const noexcept;

		void ConnectClient(NetworkSessionSlot clientSlot, std::weak_ptr<LocalClientSession> localClient);
		void ReceiveFromClient(const LocalSessionMessage &message) noexcept;

	  protected:
		[[deprecated]] ClientSessionID NewID() noexcept;
		void EnqueueMessage(ClientSessionID clientSessionID, const NetworkSessionData &data, ELocalSessionSource source);
		void Send(ClientSessionID clientSessionID, const NetworkSessionData &data, ELocalSessionSource source);
		void Broadcast(const NetworkSessionData &data, ELocalSessionSource source);
	};
} // namespace tudov
