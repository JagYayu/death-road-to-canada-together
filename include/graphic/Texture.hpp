/**
 * @file graphic/Texture.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "SDL3/SDL_render.h"

#include <cmath>
#include <cstdint>
#include <tuple>

struct SDL_GPUTexture;

namespace tudov
{
	struct ITexture
	{
		virtual ~ITexture() noexcept = default;

		virtual std::float_t GetWidth() const = 0;
		virtual std::float_t GetHeight() const = 0;
		virtual std::tuple<std::float_t, std::float_t> GetSize() const = 0;
	};

	class Renderer;
	class Image;

	class Texture : public ITexture
	{
	  protected:
		SDL_Texture *_sdlTexture;

	  public:
		Renderer &renderer;

	  public:
		explicit Texture(Renderer &renderer) noexcept;
		~Texture() noexcept override;

		std::float_t GetWidth() const override;
		std::float_t GetHeight() const override;
		std::tuple<std::float_t, std::float_t> GetSize() const override;

		void Initialize(std::int32_t width, std::int32_t height, SDL_PixelFormat format, SDL_TextureAccess access = SDL_TEXTUREACCESS_TARGET);
		void Initialize(Image &image);

		SDL_Texture *GetSDLTextureHandle() noexcept;

		inline const SDL_Texture *GetSDLTextureHandle() const noexcept
		{
			return const_cast<Texture *>(this)->GetSDLTextureHandle();
		}

	  private:
		void PostInitialize() noexcept;
	};
} // namespace tudov
