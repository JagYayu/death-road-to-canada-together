#pragma once

#include <cstdint>

namespace tudov
{
	enum class EDisconnectionCode : std::int32_t
	{
		None = 0,
		Rejected = 1,
	};
}
