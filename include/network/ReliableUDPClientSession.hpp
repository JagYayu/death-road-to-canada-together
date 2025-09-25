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
struct _ENetEvent;

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
		NetworkSessionSlot _clientSessionSlot;
		ClientSessionID _clientSessionID;
		_ENetHost *_eNetHost;
		_ENetPeer *_eNetPeer;
		bool _isConnecting;

	  public:
		explicit ReliableUDPClientSession(INetworkManager &networkManager, NetworkSessionSlot clientSlot) noexcept;
		explicit ReliableUDPClientSession(const ReliableUDPClientSession &) noexcept = default;
		explicit ReliableUDPClientSession(ReliableUDPClientSession &&) noexcept = default;
		ReliableUDPClientSession &operator=(const ReliableUDPClientSession &) noexcept = delete;
		ReliableUDPClientSession &operator=(ReliableUDPClientSession &&) noexcept = delete;
		~ReliableUDPClientSession() noexcept;

	  private:
		void TryCreateENetHost();

	  public:
		ESocketType GetSocketType() const noexcept override;
		NetworkSessionSlot GetSessionSlot() noexcept override;
		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		INetworkManager &GetNetworkManager() noexcept override;
		bool Update() override;

		ClientSessionID GetSessionID() const noexcept override;
		EClientSessionState GetSessionState() const noexcept override;
		void Connect(const IClientSession::ConnectArgs &address) override;
		void Disconnect(EDisconnectionCode code) override;
		bool TryDisconnect(EDisconnectionCode code) override;
		void SendReliable(const NetworkSessionData &data) override;
		void SendUnreliable(const NetworkSessionData &data) override;

		inline void Connect(const ConnectArgs &args)
		{
			Connect(static_cast<const IClientSession::ConnectArgs &>(args));
		}

	  private:
		void UpdateENetReceive(_ENetEvent &event) noexcept;
	};
} // namespace tudov
