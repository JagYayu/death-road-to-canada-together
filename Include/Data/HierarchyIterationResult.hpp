/**
 * @file data/HierarchyIterationResult.hpp
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
	enum class EHierarchyIterationResult
	{
		Continue = SDL_EnumerationResult::SDL_ENUM_CONTINUE,
		Success = SDL_EnumerationResult::SDL_ENUM_SUCCESS,
		Failure = SDL_EnumerationResult::SDL_ENUM_FAILURE,
	};
} // namespace tudov
