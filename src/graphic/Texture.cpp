/**
 * @file graphic/Texture.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "graphic/Texture.hpp"

#include "graphic/Image.hpp"
#include "graphic/Renderer.hpp"
#include "util/Micros.hpp"

#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"

#include <stdexcept>

using namespace tudov;

Texture::Texture(Renderer &renderer) noexcept
    : renderer(renderer),
      _sdlTexture(nullptr)
{
}

Texture::~Texture() noexcept
{
	if (_sdlTexture != nullptr)
	{
		SDL_DestroyTexture(_sdlTexture);
	}
}

TE_FORCEINLINE void AssertUninitialized(SDL_Texture *sdlTexture)
{
	if (sdlTexture != nullptr) [[unlikely]]
	{
		throw std::runtime_error("Attempt to reinitialize texture");
	}
}

TE_FORCEINLINE void AssertInitialized(SDL_Texture *sdlTexture)
{
	if (sdlTexture == nullptr) [[unlikely]]
	{
		throw std::runtime_error("Texture_depreciated was not initialized");
	}
}

void Texture::Initialize(std::int32_t width, std::int32_t height, SDL_PixelFormat format, SDL_TextureAccess access)
{
	AssertUninitialized(_sdlTexture);

	_sdlTexture = SDL_CreateTexture(renderer.GetSDLRendererHandle(), format, access, width, height);

	SDL_SetTextureScaleMode(_sdlTexture, SDL_SCALEMODE_NEAREST);
}

void Texture::Initialize(Image &image)
{
	AssertUninitialized(_sdlTexture);

	auto sdlSurface = static_cast<SDL_Surface *>(image.GetSDLSurfaceHandle());
	_sdlTexture = SDL_CreateTextureFromSurface(renderer.GetSDLRendererHandle(), sdlSurface);

	SDL_SetTextureScaleMode(_sdlTexture, SDL_SCALEMODE_NEAREST);
}

std::float_t Texture::GetWidth() const
{
	AssertInitialized(_sdlTexture);

	std::float_t width;
	SDL_GetTextureSize(_sdlTexture, &width, nullptr);
	return width;
}

std::float_t Texture::GetHeight() const
{
	AssertInitialized(_sdlTexture);

	std::float_t height;
	SDL_GetTextureSize(_sdlTexture, nullptr, &height);
	return height;
}

std::tuple<std::float_t, std::float_t> Texture::GetSize() const
{
	AssertInitialized(_sdlTexture);

	std::float_t width, height;
	SDL_GetTextureSize(_sdlTexture, &width, &height);
	return {width, height};
}

SDL_Texture *Texture::GetSDLTextureHandle() noexcept
{
	return _sdlTexture;
}
