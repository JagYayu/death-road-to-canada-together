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
	class LocalClientSession;

	/**
	 * We treat `ClientID` as `ClientUID + 1`, since all clients connected to local server are local clients.
	 */
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
			std::unordered_map<ClientSessionToken, std::weak_ptr<LocalClientSession>> localClients;
		};

	  protected:
		INetworkManager &_networkManager;
		EServerSessionState _sessionState;
		std::uint32_t _serverUID;
		std::unique_ptr<HostInfo> _hostInfo;
		std::queue<LocalSessionMessage> _messageQueue;
		ClientSessionToken _latestToken;

	  public:
		explicit LocalServerSession(INetworkManager &networkManager, std::uint32_t serverUID) noexcept;
		~LocalServerSession() noexcept override = default;

	  public:
		INetworkManager &GetNetworkManager() noexcept override;
		ESocketType GetSocketType() const noexcept override;
		Log &GetLog() noexcept override;

		EServerSessionState GetSessionState() const noexcept override;
		void Host(const HostArgs &args) override;
		void Shutdown() override;
		bool TryShutdown() override;
		std::optional<std::string_view> GetTitle() const noexcept override;
		std::optional<std::string_view> GetPassword() const noexcept override;
		std::optional<std::size_t> GetMaxClients() const noexcept override;

		void SendReliable(ClientSessionToken clientSessionToken, std::span<const std::byte> data, ChannelID channelID) override;
		void SendUnreliable(ClientSessionToken clientSessionToken, std::span<const std::byte> data, ChannelID channelID) override;
		void BroadcastReliable(std::span<const std::byte> data, ChannelID channelID) override;
		void BroadcastUnreliable(std::span<const std::byte> data, ChannelID channelID) override;

		bool Update() override;

		ClientSessionToken ClientUIDToClientID(std::uint32_t clientUID) const noexcept;

		void Receive(const LocalSessionMessage &message) noexcept;

		void AddClient(std::uint32_t clientUID, std::weak_ptr<LocalClientSession> localClient);

	  protected:
		ClientSessionToken NewToken() noexcept;
		void EnqueueMessage(ClientSessionToken clientSessionToken, std::span<const std::byte> data, ChannelID channelID, ELocalSessionSource source);
	};
} // namespace tudov
