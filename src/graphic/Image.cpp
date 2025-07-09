#include "Image.hpp"

#include "SDL3/SDL_Surface.h"
#include "SDL3/SDL_error.h"
#include "SDL3_image/SDL_image.h"

using namespace tudov;

Image::Image() noexcept
{
}

Image::~Image() noexcept
{
	if (_sdlSurface)
	{
		SDL_DestroySurface(_sdlSurface);
	}
}

bool Image::IsValid() noexcept
{
	return _sdlSurface != nullptr;
}

void Image::Initialize(std::string_view memory) noexcept
{
	SDL_IOStream *rw = SDL_IOFromConstMem(memory.data(), memory.size());
	if (!rw)
	{
		SDL_Log("SDL_IOFromConstMem failed: %s", SDL_GetError());
		return;
	}

	_sdlSurface = IMG_Load_IO(rw, 1);
	if (!_sdlSurface)
	{
		SDL_Log("IMG_Load_IO failed: %s", SDL_GetError());
		return;
	}
}

SDL_Surface *Image::GetSDLSurfaceHandle() noexcept
{
	return _sdlSurface;
}

const SDL_Surface *Image::GetSDLSurfaceHandle() const noexcept
{
	return _sdlSurface;
}
