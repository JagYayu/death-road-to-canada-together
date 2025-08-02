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
