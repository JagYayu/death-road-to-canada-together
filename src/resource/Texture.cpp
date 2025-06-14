#include "Texture.h"

#include "graphic/Renderer.h"
#include "graphic/Window.h"
#include "util/Defs.h"

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include <SDL3_image/SDL_image.h>

#include <exception>

using namespace tudov;

Texture::Texture(Renderer &renderer, StringView data)
{
	SDL_IOStream *rw = SDL_IOFromConstMem(data.data(), data.size());
	SDL_Surface *surf = IMG_Load_IO(rw, 1);
	if (!surf)
	{
		throw std::exception("Failed to load texture");
	}

	auto ptr = renderer.GetRaw();

	SDL_Texture *tex = SDL_CreateTextureFromSurface(ptr, surf);
	SDL_DestroySurface(surf);

	_texture = SDL_CreateTextureFromSurface(ptr, surf);
}

Texture::~Texture() noexcept
{
	if (_texture)
	{
		SDL_DestroyTexture(_texture);
		_texture = nullptr;
	}
}
