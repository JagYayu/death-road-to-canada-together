#pragma once

#include "../ITexture.h"
#include "graphic/ERenderBackend.h"
#include "util/Defs.h"

#include "SDL3/SDL_render.h"
#include <functional>
#include <memory>

struct SDL_Texture;

namespace tudov
{
	class SDLRenderer;
	class SDLSurface;
	class Window;

	class SDLTexture : public ITexture
	{
	  private:
		SDL_Texture *_texture;

	  public:
		// SDLTexture(SDLRenderer &renderer, std::string_view data);
		SDLTexture(SDLRenderer &renderer, const std::shared_ptr<SDLSurface> &surface);
		~SDLTexture() noexcept;

		SDL_Texture *GetRaw() noexcept;
		const SDL_Texture *GetRaw() const noexcept;

		ERenderBackend GetRenderBackend() const noexcept override;

		std::optional<std::reference_wrapper<SDLSurface>> GetSurface() noexcept;
	};
} // namespace tudov
