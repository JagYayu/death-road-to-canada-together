/**
 * @file network/LocalClient.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "ClientSession.hpp"
#include "LocalSession.hpp"
#include "System/Log.hpp"
#include "util/Definitions.hpp"

#include <memory>
#include <queue>

namespace tudov
{
	struct NetworkSessionData;
	class LocalServerSession;
	class LocalSession;

	class LocalClientSession : public IClientSession, private ILogProvider, public std::enable_shared_from_this<LocalClientSession>
	{
		// friend LocalServerSession;
		friend LocalSession;

	  public:
		struct ConnectArgs : public IClientSession::ConnectArgs
		{
			LocalServerSession *localServer;
		};

	  protected:
		INetworkManager &_networkManager;
		EClientSessionState _clientSessionState;
		NetworkSessionSlot _clientSessionSlot;
		ClientSessionID _clientSessionID = 0;
		LocalServerSession *_localServer;
		std::queue<LocalSessionMessage> _messageQueue;

	  public:
		explicit LocalClientSession(INetworkManager &network, NetworkSessionSlot clientSlot) noexcept;
		explicit LocalClientSession(const LocalClientSession &) noexcept = default;
		explicit LocalClientSession(LocalClientSession &&) noexcept = default;
		LocalClientSession &operator=(const LocalClientSession &) noexcept = delete;
		LocalClientSession &operator=(LocalClientSession &&) noexcept = delete;
		~LocalClientSession() noexcept override = default;

	  public:
		INetworkManager &GetNetworkManager() noexcept override;
		NetworkSessionSlot GetSessionSlot() noexcept override;
		ESocketType GetSocketType() const noexcept override;
		Log &GetLog() noexcept override;

		ClientSessionID GetSessionID() const noexcept override;
		EClientSessionState GetSessionState() const noexcept override;
		void Connect(const IClientSession::ConnectArgs &args) override;
		void Disconnect(EDisconnectionCode code) override;
		bool TryDisconnect(EDisconnectionCode code) override;
		void SendReliable(const NetworkSessionData &data) override;
		void SendUnreliable(const NetworkSessionData &data) override;

		bool Update() override;

		NetworkSessionSlot GetClientSlot() noexcept;

		/**
		 * Receive data from `LocalServerSession`.
		 */
		void ReceiveFromServer(const LocalSessionMessage &message) noexcept;

		inline void Connect(const ConnectArgs &args)
		{
			Connect(static_cast<const IClientSession::ConnectArgs &>(args));
		}

		inline void ConnectAsync(const ConnectArgs &args, const ClientHostErrorHandler &handler) noexcept
		{
			Connect(static_cast<const IClientSession::ConnectArgs &>(args));
		}

	  private:
		void UpdateReceive(LocalSessionMessage &messageEntry) noexcept;
	};
} // namespace tudov
