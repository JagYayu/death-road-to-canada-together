/**
 * @file data/PathType.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "SDL3/SDL_filesystem.h"

namespace tudov
{
	enum class EPathType : int
	{
		None = SDL_PATHTYPE_NONE,
		Directory = SDL_PATHTYPE_DIRECTORY,
		File = SDL_PATHTYPE_FILE,
		Other = SDL_PATHTYPE_OTHER,
	};

	static_assert(sizeof(EPathType) == sizeof(SDL_PathType), "`EPathType` and `SDL_PathType` don't have the same size!");
} // namespace tudov
