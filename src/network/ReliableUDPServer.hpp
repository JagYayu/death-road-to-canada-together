#pragma once

#include "Server.hpp"

namespace tudov
{
	class ReliableUDPServer : public IServer
	{
	  public:
		explicit ReliableUDPServer(INetwork &network) noexcept;
	};
} // namespace tudov
