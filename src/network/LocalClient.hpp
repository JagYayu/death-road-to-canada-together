#pragma once

#include "Client.hpp"

#include <memory>

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

	  private:
		INetwork &_network;
		bool _connected;

	  public:
		explicit LocalClient(INetwork &network) noexcept;
		~LocalClient() noexcept override = default;

	  public:
		INetwork &GetNetwork() noexcept override;
		ESocketType GetSocketType() const noexcept override;

		bool IsConnecting() noexcept override;
		bool IsConnected() noexcept override;
		void Connect(const IClient::ConnectArgs &args) override;
		void Disconnect() override;
		void SendReliable(std::string_view data) override;
		void SendUnreliable(std::string_view data) override;

		bool Update() override;

		inline void Connect(const ConnectArgs &args)
		{
			Connect(static_cast<const IClient::ConnectArgs &>(args));
		}
	};
} // namespace tudov
