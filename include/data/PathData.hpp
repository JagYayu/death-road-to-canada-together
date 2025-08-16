/**
 * @file data/PathData.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "PathInfo.hpp"

#include <cstddef>
#include <vector>

namespace tudov
{
	struct PathData
	{
		PathInfo info;
		std::vector<std::byte> byte;
	};
} // namespace tudov
