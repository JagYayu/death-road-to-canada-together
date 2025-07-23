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