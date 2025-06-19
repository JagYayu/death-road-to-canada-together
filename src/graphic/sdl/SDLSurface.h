#pragma once

#include "graphic/ERenderBackend.h"
#include "graphic/ISurface.h"
#include "util/Defs.h"

#include "SDL3/SDL_render.h"
#include "glm/ext/vector_float2.hpp"

#include <memory>

struct SDL_Surface;

namespace tudov
{
	class SDLRenderer;

	class SDLSurface : public ISurface
	{
	  private:
		SDL_Surface *_surface;

	  public:
		explicit SDLSurface(SDLRenderer &renderer, std::string_view data);
		~SDLSurface() noexcept;

	  private:
		Color GetPixelFromPointer(const std::uint8_t *ptr) const noexcept;

	  public:
		SDL_Surface *GetRaw() noexcept;
		const SDL_Surface *GetRaw() const noexcept;

		std::int32_t GetWidth() const noexcept override;
		std::int32_t GetHeight() const noexcept override;
		Color GetPixel(std::int32_t index) const noexcept override;
		Color GetPixel(std::int32_t x, std::int32_t y) const noexcept override;
	};
} // namespace tudov
