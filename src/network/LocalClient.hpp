#pragma once

#include "Client.hpp"

namespace tudov
{
	class LocalClient : public IClient
	{
	  public:
		struct LocalConnectArgs : public ConnectArgs
		{
		};

	  private:
		INetwork &_network;
		bool _connected;

	  public:
		explicit LocalClient(INetwork &network) noexcept;
		~LocalClient() noexcept = default;

	  public:
		INetwork &GetNetwork() noexcept override;
		ESocketType GetSocketType() const noexcept override;

		bool IsConnecting() noexcept override;
		bool IsConnected() noexcept override;
		void Connect(const ConnectArgs &args) override;
		void Disconnect() override;

		bool Update() override;

		inline void Connect(const LocalConnectArgs &args)
		{
			Connect(static_cast<const ConnectArgs &>(args));
		}
	};
} // namespace tudov
