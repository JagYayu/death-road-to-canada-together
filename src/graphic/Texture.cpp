#include "Texture.h"

#include "Renderer.h"
#include "SDL3/SDL_pixels.h"
#include "Image.h"

#include "SDL3/SDL_render.h"
#include "SDL3_image/SDL_image.h"

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

void Texture::Initialize(std::int32_t width, std::int32_t height, SDL_PixelFormat format, SDL_TextureAccess access) noexcept
{
	_sdlTexture = SDL_CreateTexture(renderer.GetSDLRendererHandle(), format, access, width, height);
}

void Texture::Initialize(Image &image) noexcept
{
	_sdlTexture = SDL_CreateTextureFromSurface(renderer.GetSDLRendererHandle(), image.GetSDLSurfaceHandle());
}

SDL_Texture *Texture::GetSDLTextureHandle() noexcept
{
	return _sdlTexture;
}

const SDL_Texture *Texture::GetSDLTextureHandle() const noexcept
{
	return _sdlTexture;
}
