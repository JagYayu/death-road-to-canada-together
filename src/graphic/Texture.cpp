#include "Texture.hpp"

#include "Image.hpp"
#include "Renderer.hpp"

#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3_image/SDL_image.h"
#include "util/Micros.hpp"
#include <stdexcept>

using namespace tudov;

Texture::Texture(Renderer &renderer) noexcept
    : renderer(renderer)
{
}

Texture::~Texture() noexcept
{
	if (_sdlTexture)
	{
		SDL_DestroyTexture(_sdlTexture);
	}
}

TE_FORCEINLINE void AssertInitialization(SDL_Texture *sdlTexture)
{
	if (sdlTexture == nullptr) [[unlikely]]
	{
		throw std::runtime_error("Texture was not initialized");
	}
}

void Texture::Initialize(std::int32_t width, std::int32_t height, SDL_PixelFormat format, SDL_TextureAccess access) noexcept
{
	_sdlTexture = SDL_CreateTexture(renderer.GetSDLRendererHandle(), format, access, width, height);
}

void Texture::Initialize(Image &image) noexcept
{
	_sdlTexture = SDL_CreateTextureFromSurface(renderer.GetSDLRendererHandle(), image.GetSDLSurfaceHandle());
}

std::float_t Texture::GetWidth() const
{
	AssertInitialization(_sdlTexture);

	std::float_t width;
	SDL_GetTextureSize(_sdlTexture, &width, nullptr);
	return width;
}

std::float_t Texture::GetHeight() const
{
	AssertInitialization(_sdlTexture);

	std::float_t height;
	SDL_GetTextureSize(_sdlTexture, nullptr, &height);
	return height;
}

std::tuple<std::float_t, std::float_t> Texture::GetSize() const
{
	AssertInitialization(_sdlTexture);

	std::float_t width, height;
	SDL_GetTextureSize(_sdlTexture, &width, &height);
	return {width, height};
}

SDL_Texture *Texture::GetSDLTextureHandle() noexcept
{
	return _sdlTexture;
}
