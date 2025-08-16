/**
 * @file graphic/Vertex.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "util/Color.hpp"

#include <cmath>

namespace tudov
{
	struct Vertex
	{
		std::float_t x, y, z;
		Color color;
		std::float_t tx, ty;
		std::uint32_t anim;
	};
} // namespace tudov