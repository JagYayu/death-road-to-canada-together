#pragma once

#include "../ITexture.h"
#include "util/Defs.h"

#include "SDL3/SDL_render.h"

namespace tudov
{
	class SDLRenderer;
	class Window;

	class SDLTexture : public ITexture
	{
	  private:
		SDL_Texture *_texture;

	  public:
		SDLTexture(SDLRenderer &renderer, std::string_view data);
		~SDLTexture() noexcept;

		SDL_Texture *GetRaw() noexcept;
		const SDL_Texture *GetRaw() const noexcept;
	};
} // namespace tudov
