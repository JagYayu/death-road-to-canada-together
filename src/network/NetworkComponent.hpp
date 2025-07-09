#pragma once

#include "SocketType.hpp"
#include "program/Context.hpp"

namespace tudov
{
	struct INetwork;

	struct INetworkComponent : public ISocketTypeProvider, public IContextProvider
	{
		virtual INetwork &GetNetwork() noexcept = 0;
		virtual bool Update() = 0;

		Context &GetContext() noexcept override;
	};
} // namespace tudov
