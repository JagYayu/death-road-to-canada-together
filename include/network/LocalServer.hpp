/**
 * @file network/LocalServer.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Server.hpp"
#include "SocketType.hpp"
#include "data/Constants.hpp"
#include "system/Log.hpp"

#include <memory>
#include <queue>
#include <string>
#include <unordered_map>

namespace tudov
{
	struct INetworkManager;
	struct INetworkManager;
	class LocalClient;

	class LocalServer : public IServer
	{
	  public:
		using HostArgs = IServer::HostArgs;

	  protected:
		struct HostInfo
		{
			std::string_view title = NetworkServerTitle;
			std::string_view password = NetworkServerPassword;
			std::size_t maximumClients = NetworkServerMaximumClients;
			std::unordered_map<ClientID, std::weak_ptr<LocalClient>> localClients;
		};

		struct Message
		{
			enum ESource
			{
				SendReliable,
				SendUnreliable,
				BroadcastReliable,
				BroadcastUnreliable,
			};

			ESource source;
			ClientID clientID;
			std::string data;

			explicit Message(bool reliable, std::string data) noexcept;
			explicit Message(bool reliable, ClientID clientID, std::string data) noexcept;
			~Message() noexcept = default;
		};

	  protected:
		INetworkManager &_networkManager;
		std::unique_ptr<HostInfo> _hostInfo;
		std::queue<Message> _messageQueue;

	  public:
		explicit LocalServer(INetworkManager &networkManager) noexcept;
		~LocalServer() noexcept override = default;

	  public:
		INetworkManager &GetNetworkManager() noexcept override;
		ESocketType GetSocketType() const noexcept override;

		bool IsHosting() const noexcept override;
		void Host(const HostArgs &args) override;
		void Shutdown() override;
		std::optional<std::string_view> GetTitle() const noexcept override;
		std::optional<std::string_view> GetPassword() const noexcept override;
		std::optional<std::size_t> GetMaxClients() const noexcept override;

		void SendReliable(ClientID clientID, std::string_view data) override;
		void SendUnreliable(ClientID clientID, std::string_view data) override;
		void BroadcastReliable(std::string_view data) override;
		void BroadcastUnreliable(std::string_view data) override;

		bool Update() override;

		void AddClient(std::uint32_t uid, const std::weak_ptr<LocalClient> &localClient);
	};
} // namespace tudov
