#pragma once

#include "SDL3/SDL_render.h"
#include "util/Defs.h"

namespace tudov
{
	class Renderer;
	class Window;

	class Texture
	{
		friend Renderer;

	  private:
		SDL_Texture *_texture;

	  public:
		Texture(const Renderer &renderer, StringView data);
		~Texture() noexcept;
	};
} // namespace tudov
