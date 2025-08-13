#pragma once

#include <cstdint>

namespace tudov
{
	enum class EHierarchyElement : std::uint8_t
	{
		All = static_cast<std::uint8_t>(-1),
		None = 0,
		Data = 1 << 0,
		Directory = 1 << 1,
	};
} // namespace tudov
