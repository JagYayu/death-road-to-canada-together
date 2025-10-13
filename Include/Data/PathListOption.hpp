#pragma once

#include <cstdint>

namespace tudov
{
	enum class EPathListOption : std::uint8_t
	{
		All = static_cast<std::uint8_t>(-1),
		None = 0,
		File = 1 << 0,
		Directory = 1 << 1,
		Recursed = 1 << 2,
		Sorted = 1 << 3,
		FullPath = 1 << 4,

		Default = File | Directory | Sorted | FullPath,
	};
} // namespace tudov
