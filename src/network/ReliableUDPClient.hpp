#pragma once

#include "Client.hpp"
#include "SocketType.hpp"
#include <string_view>

struct _ENetHost;
struct _ENetPeer;

namespace tudov
{
	class ReliableUDPClient : public IClient
	{
	  public:
		struct ReliableUDPConnectArgs : public ConnectArgs
		{
			std::string_view host;
			std::uint16_t port;
		};

	  protected:
		Context &_context;
		_ENetHost *_eNetHost;
		_ENetPeer *_eNetPeer;
		bool _isConnecting;

	  public:
		explicit ReliableUDPClient(Context &context) noexcept;
		~ReliableUDPClient() noexcept;

	  private:
		void TryCreateENetHost();

	  public:
		Context &GetContext() noexcept override;
		ESocketType GetSocketType() const noexcept override;

		bool IsConnecting() noexcept override;
		bool IsConnected() noexcept override;
		void Connect(const ConnectArgs &address) override;
		void Disconnect() override;

		bool Update() override;

		void Connect(const ReliableUDPConnectArgs &args)
		{
		}
	};
} // namespace tudov
