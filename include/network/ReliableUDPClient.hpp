/**
 * @file network/ReliableUDPClient.hpp
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
#include "SocketType.hpp"

#include <string_view>

struct _ENetHost;
struct _ENetPeer;

namespace tudov
{
	struct INetworkManager;

	class ReliableUDPClient : public IClient
	{
	  public:
		struct ConnectArgs : public IClient::ConnectArgs
		{
			std::string_view host;
			std::uint16_t port;
		};

	  protected:
		INetworkManager &_networkManager;
		_ENetHost *_eNetHost;
		_ENetPeer *_eNetPeer;
		bool _isConnecting;

	  public:
		explicit ReliableUDPClient(INetworkManager &networkManager) noexcept;
		~ReliableUDPClient() noexcept;

	  private:
		void TryCreateENetHost();

	  public:
		ESocketType GetSocketType() const noexcept override;
		Context &GetContext() noexcept override;
		INetworkManager &GetNetworkManager() noexcept override;
		bool Update() override;

		bool IsConnecting() const noexcept override;
		bool IsConnected() const noexcept override;
		void Connect(const IClient::ConnectArgs &address) override;
		void Disconnect() override;
		void SendReliable(std::string_view data) override;
		void SendUnreliable(std::string_view data) override;

		inline void Connect(const ConnectArgs &args)
		{
			Connect(static_cast<const IClient::ConnectArgs &>(args));
		}
	};
} // namespace tudov
