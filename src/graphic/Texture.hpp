#pragma once

#include "SDL3/SDL_render.h"

#include <cmath>
#include <cstdint>
#include <tuple>

namespace tudov
{
	class Renderer;
	class Image;

	class Texture
	{
	  private:
		SDL_Texture *_sdlTexture;

	  public:
		Renderer &renderer;

	  public:
		explicit Texture(Renderer &renderer) noexcept;
		~Texture() noexcept;

		void Initialize(std::int32_t width, std::int32_t height, SDL_PixelFormat format, SDL_TextureAccess access = SDL_TEXTUREACCESS_TARGET) noexcept;
		void Initialize(Image &image) noexcept;

		std::float_t GetWidth() const;
		std::float_t GetHeight() const;
		std::tuple<std::float_t, std::float_t> GetSize() const;

		SDL_Texture *GetSDLTextureHandle() noexcept;

		inline const SDL_Texture *GetSDLTextureHandle() const noexcept
		{
			return const_cast<Texture *>(this)->GetSDLTextureHandle();
		}
	};
} // namespace tudov
