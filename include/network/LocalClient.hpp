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

#include "Client.hpp"

#include <memory>
#include <queue>
#include <string>

namespace tudov
{
	class LocalServer;

	class LocalClient : public IClient, public std::enable_shared_from_this<LocalClient>
	{
	  public:
		struct ConnectArgs : public IClient::ConnectArgs
		{
			std::int32_t user;
			LocalServer *server;
		};

	  protected:
		INetworkManager &_networkManager;
		bool _connected;
		std::queue<std::string> _messageQueue;

	  public:
		explicit LocalClient(INetworkManager &network) noexcept;
		~LocalClient() noexcept override = default;

	  public:
		INetworkManager &GetNetworkManager() noexcept override;
		ESocketType GetSocketType() const noexcept override;

		bool IsConnecting() noexcept override;
		bool IsConnected() noexcept override;
		void Connect(const IClient::ConnectArgs &args) override;
		void Disconnect() override;
		void SendReliable(std::string_view data) override;
		void SendUnreliable(std::string_view data) override;

		bool Update() override;

		void Receive(std::string_view data) noexcept;

		inline void Connect(const ConnectArgs &args)
		{
			Connect(static_cast<const IClient::ConnectArgs &>(args));
		}
	};
} // namespace tudov
