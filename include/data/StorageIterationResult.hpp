#pragma once

#include "SDL3/SDL_filesystem.h"

namespace tudov
{
	enum class EStorageIterationResult
	{
		Continue = SDL_EnumerationResult::SDL_ENUM_CONTINUE,
		Success = SDL_EnumerationResult::SDL_ENUM_SUCCESS,
		Failure = SDL_EnumerationResult::SDL_ENUM_FAILURE,
	};
} // namespace tudov
