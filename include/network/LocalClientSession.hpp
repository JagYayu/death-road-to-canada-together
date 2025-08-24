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
#include "system/Log.hpp"
#include "util/Definitions.hpp"

#include <cstddef>
#include <memory>
#include <queue>

namespace tudov
{
	struct NetworkSessionData;
	class LocalServerSession;

	class LocalClientSession : public IClientSession, private ILogProvider, public std::enable_shared_from_this<LocalClientSession>
	{
		friend LocalServerSession;

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
		~LocalClientSession() noexcept override = default;

	  public:
		INetworkManager &GetNetworkManager() noexcept override;
		NetworkSessionSlot GetSessionSlot() noexcept override;
		ESocketType GetSocketType() const noexcept override;
		Log &GetLog() noexcept override;

		ClientSessionID GetSessionID() const noexcept override;
		EClientSessionState GetSessionState() const noexcept override;
		void Connect(const IClientSession::ConnectArgs &args) override;
		void Disconnect() override;
		bool TryDisconnect() override;
		void SendReliable(const NetworkSessionData& data) override;
		void SendUnreliable(const NetworkSessionData& data) override;

		bool Update() override;

		NetworkSessionSlot GetClientSlot() noexcept;

		/**
		 * Receive data from `LocalServerSession`.
		 */
		void Receive(const LocalSessionMessage &message) noexcept;

		inline void Connect(const ConnectArgs &args)
		{
			Connect(static_cast<const IClientSession::ConnectArgs &>(args));
		}
	};
} // namespace tudov
