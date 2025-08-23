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

#include "ClientSession.hpp"
#include "SocketType.hpp"
#include "system/Log.hpp"

#include <string_view>

struct _ENetHost;
struct _ENetPeer;

namespace tudov
{
	struct INetworkManager;

	class ReliableUDPClientSession : public IClientSession, private ILogProvider
	{
	  public:
		struct ConnectArgs : public IClientSession::ConnectArgs
		{
			std::string_view host;
			std::uint16_t port;
		};

	  protected:
		INetworkManager &_networkManager;
		ClientSessionToken _clientToken;
		_ENetHost *_eNetHost;
		_ENetPeer *_eNetPeer;
		bool _isConnecting;

	  public:
		explicit ReliableUDPClientSession(INetworkManager &networkManager) noexcept;
		~ReliableUDPClientSession() noexcept;

	  private:
		void TryCreateENetHost();

	  public:
		ESocketType GetSocketType() const noexcept override;
		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		INetworkManager &GetNetworkManager() noexcept override;
		bool Update() override;

		ClientSessionToken GetToken() const noexcept override;
		EClientSessionState GetSessionState() const noexcept override;
		void Connect(const IClientSession::ConnectArgs &address) override;
		void Disconnect() override;
		bool TryDisconnect() override;
		void SendReliable(std::span<const std::byte> data, ChannelID channelID) override;
		void SendUnreliable(std::span<const std::byte> data, ChannelID channelID) override;

		inline void Connect(const ConnectArgs &args)
		{
			Connect(static_cast<const IClientSession::ConnectArgs &>(args));
		}
	};
} // namespace tudov
