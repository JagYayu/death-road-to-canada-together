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
		ClientSessionToken _clientSessionToken = 0;
		std::uint32_t _clientUID;
		LocalServerSession *_localServer;
		std::queue<LocalSessionMessage> _messageQueue;

	  public:
		explicit LocalClientSession(INetworkManager &network, std::uint32_t clientUID) noexcept;
		~LocalClientSession() noexcept override = default;

	  public:
		INetworkManager &GetNetworkManager() noexcept override;
		ESocketType GetSocketType() const noexcept override;
		Log &GetLog() noexcept override;

		ClientSessionToken GetToken() const noexcept override;
		EClientSessionState GetSessionState() const noexcept override;
		void Connect(const IClientSession::ConnectArgs &args) override;
		void Disconnect() override;
		bool TryDisconnect() override;
		void SendReliable(std::span<const std::byte> data, ChannelID channelID) override;
		void SendUnreliable(std::span<const std::byte> data, ChannelID channelID) override;

		bool Update() override;

		std::uint32_t GetClientUID() noexcept;

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
