/**
 * @file data/PathInfo.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

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
