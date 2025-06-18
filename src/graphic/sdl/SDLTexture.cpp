#include "SDLTexture.h"

#include "../IRenderer.h"
#include "../ITexture.h"
#include "../Window.h"
#include "SDLRenderer.h"


#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "graphic/ERenderBackend.h"
#include <SDL3_image/SDL_image.h>

#include <cassert>
#include <exception>

using namespace tudov;

SDLTexture::SDLTexture(SDLRenderer &renderer, std::string_view data)
    : ITexture(renderer)
{
	SDL_IOStream *rw = SDL_IOFromConstMem(data.data(), data.size());
	SDL_Surface *surf = IMG_Load_IO(rw, 1);
	if (surf)
	{
		auto &&raw = renderer.GetRaw();
		assert(raw);
		_texture = SDL_CreateTextureFromSurface(raw, surf);
		SDL_DestroySurface(surf);
	}

	if (!_texture)
	{
		auto &&msg = std::format("Failed to load texture: \"{}\"", std::string_view(SDL_GetError()));
		throw std::exception(msg.c_str());
	}
}

SDLTexture::~SDLTexture() noexcept
{
	if (_texture)
	{
		SDL_DestroyTexture(_texture);
		_texture = nullptr;
	}
}

SDL_Texture *SDLTexture::GetRaw() noexcept
{
	return _texture;
}

const SDL_Texture *SDLTexture::GetRaw() const noexcept
{
	return _texture;
}

ERenderBackend SDLTexture::GetRenderBackend() const noexcept
{
	return ERenderBackend::SDL;
}
