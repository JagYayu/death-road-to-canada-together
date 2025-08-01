#include "graphic/Image.hpp"

#include "SDL3/SDL_Surface.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_log.h"
#include "SDL3_image/SDL_image.h"

#include <format>
#include <stdexcept>

using namespace tudov;

Image::Image(const std::vector<std::byte> &bytes)
{
	SDL_IOStream *io = SDL_IOFromConstMem(bytes.data(), bytes.size());
	if (!io) [[unlikely]]
	{
		throw std::runtime_error(std::format("SDL_IOFromConstMem failed: {}", SDL_GetError()));
	}

	_sdlSurface = IMG_Load_IO(io, true);
	if (!_sdlSurface) [[unlikely]]
	{
		throw std::runtime_error(std::format("IMG_Load_IO failed: {}", SDL_GetError()));
	}
}

Image::~Image() noexcept
{
	if (_sdlSurface)
	{
		SDL_DestroySurface(static_cast<SDL_Surface *>(_sdlSurface));
	}
}

bool Image::IsValid() noexcept
{
	return _sdlSurface != nullptr;
}

void Image::Initialize(std::string_view memory) noexcept
{
	if (_sdlSurface != nullptr)
	{
		return;
	}

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

impl::SDL_Surface *Image::GetSDLSurfaceHandle() noexcept
{
	return static_cast<SDL_Surface *>(_sdlSurface);
}

const impl::SDL_Surface *Image::GetSDLSurfaceHandle() const noexcept
{
	return static_cast<SDL_Surface *>(_sdlSurface);
}
