#pragma once

#include "Client.h"
#include "SocketType.h"

struct _ENetHost;

namespace tudov
{
	class ReliableUDPClient : IClient
	{
	  private:
		Context &_context;

		_ENetHost *_eNetHost;

	  public:
		explicit ReliableUDPClient(Context &context) noexcept;
		~ReliableUDPClient() noexcept;

		Context &GetContext() noexcept override;
		ESocketType GetSocketType() const noexcept override;
		void Connect(std::string_view address) override;
	};
} // namespace tudov
