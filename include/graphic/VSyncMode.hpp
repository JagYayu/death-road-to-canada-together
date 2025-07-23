#pragma once

#include "SDL3/SDL_render.h"
namespace tudov
{
	enum class EVSyncMode
	{
		Adaptive = SDL_RENDERER_VSYNC_ADAPTIVE,
		Disabled = SDL_RENDERER_VSYNC_DISABLED,
		Enabled = 1,
		EnabledHalf = 2,
	};
} // namespace tudov
