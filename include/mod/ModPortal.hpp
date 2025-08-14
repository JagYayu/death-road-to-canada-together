#pragma once

#include <cstdint>

namespace tudov
{
	enum class EModPortal : std::uint8_t
	{
		ModIO,
	};

	struct IModPortal
	{
		virtual EModPortal GetModPortalType() const noexcept = 0;
	};
} // namespace tudov
