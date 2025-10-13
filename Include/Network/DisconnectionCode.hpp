#pragma once

#include <cstdint>

namespace tudov
{
	enum class EDisconnectionCode : std::uint32_t
	{
		Unknown = 0,
		ClientClosed = 1,
		ServerClosed = 2,
	};
}
