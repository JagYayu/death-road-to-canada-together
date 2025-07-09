#pragma once

#include "SDL3/SDL_render.h"

#include <cstdint>

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

		SDL_Texture *GetSDLTextureHandle() noexcept;
		const SDL_Texture *GetSDLTextureHandle() const noexcept;
	};
} // namespace tudov
