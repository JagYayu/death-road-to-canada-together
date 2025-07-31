#pragma once

#include "PathType.hpp"

#include <cstdint>

namespace tudov
{
	/**
	 * Has the same memory layout with `SDL_PathInfo`
	 */
	struct PathInfo
	{
		EPathType type;
		std::uint64_t size;
		std::int64_t createTimeNS;
		std::int64_t modifyTimeNS;
		std::int64_t accessTimeNS;
	};
} // namespace tudov
