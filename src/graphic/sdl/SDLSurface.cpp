#include "SDLSurface.h"

#include "SDL3/SDL_pixels.h"
#include "SDL3_image/SDL_image.h"
#include "SDLRenderer.h"

#include "SDL3/SDL_surface.h"

using namespace tudov;

// SDLTexture::SDLTexture(SDLRenderer &renderer, std::string_view data)
//     : ITexture(renderer)
// {
// 	SDL_IOStream *rw = SDL_IOFromConstMem(data.data(), data.size());
// 	SDL_Surface *surf = IMG_Load_IO(rw, 1);
// 	if (surf)
// 	{
// 		auto &&raw = renderer.GetRaw();
// 		assert(raw);
// 		_texture = SDL_CreateTextureFromSurface(raw, surf);
// 		SDL_DestroySurface(surf);
// 	}

// 	if (!_texture)
// 	{
// 		auto &&msg = std::format("Failed to load texture: \"{}\"", std::string_view(SDL_GetError()));
// 		throw std::exception(msg.c_str());
// 	}
// }

SDLSurface::SDLSurface(SDLRenderer &renderer, std::string_view data)
    : ISurface(renderer)
{
	SDL_IOStream *rw = SDL_IOFromConstMem(data.data(), data.size());
	_surface = IMG_Load_IO(rw, 1);

	if (!_surface)
	{
		auto &&msg = std::format("Failed to load surface: \"{}\"", std::string_view(SDL_GetError()));
		throw std::exception(msg.c_str());
	}
}

SDLSurface::~SDLSurface() noexcept
{
	if (_surface)
	{
		SDL_DestroySurface(_surface);
	}
}

SDL_Surface *SDLSurface::GetRaw() noexcept
{
	return _surface;
}

const SDL_Surface *SDLSurface::GetRaw() const noexcept
{
	return _surface;
}

std::int32_t SDLSurface::GetWidth() const noexcept
{
	return _surface->w;
}

std::int32_t SDLSurface::GetHeight() const noexcept
{
	return _surface->h;
}

Color SDLSurface::GetPixelFromPointer(const std::uint8_t *ptr) const noexcept
{
	Color col{0, 0, 0, 0};
	if (!_surface || !ptr)
	{
		return col;
	}

	const std::uint8_t bpp = SDL_BYTESPERPIXEL(_surface->format);
	Uint32 pixel = 0;
	switch (bpp)
	{
	case 1:
		pixel = *ptr;
		break;
	case 2:
		pixel = *reinterpret_cast<const Uint16 *>(ptr);
		break;
	case 3:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		pixel = ptr[0] << 16 | ptr[1] << 8 | ptr[2];
#else
		pixel = ptr[0] | ptr[1] << 8 | ptr[2] << 16;
#endif
		break;
	case 4:
		pixel = *reinterpret_cast<const Uint32 *>(ptr);
		break;
	default:
		return col;
	}

	auto fmt = SDL_GetPixelFormatDetails(_surface->format);
	if (!fmt)
	{
		return col;
	}

	SDL_Palette *pal = SDL_GetSurfacePalette(_surface);
	SDL_GetRGBA(pixel, fmt, pal, &col.r, &col.g, &col.b, &col.a);
	return col;
}

Color SDLSurface::GetPixel(std::int32_t index) const noexcept
{
	Color col{0, 0, 0, 0};
	if (!_surface)
	{
		return col;
	}
	if (index < 0 || index >= _surface->w * _surface->h)
	{
		return col;
	}
	if (SDL_MUSTLOCK(_surface) && SDL_LockSurface(_surface) != 0)
	{
		return col;
	}

	const std::uint8_t bpp = SDL_BYTESPERPIXEL(_surface->format);
	const std::uint8_t *ptr = static_cast<const std::uint8_t *>(_surface->pixels) + index * bpp;
	col = GetPixelFromPointer(ptr);

	if (SDL_MUSTLOCK(_surface))
	{
		SDL_UnlockSurface(_surface);
	}

	return col;
}

Color SDLSurface::GetPixel(std::int32_t x, std::int32_t y) const noexcept
{
	Color col{0, 0, 0, 0};
	if (!_surface || x < 0 || y < 0 || x >= _surface->w || y >= _surface->h)
	{
		return col;
	}
	if (SDL_MUSTLOCK(_surface) && SDL_LockSurface(_surface) != 0)
	{
		return col;
	}

	const std::uint8_t bpp = SDL_BYTESPERPIXEL(_surface->format);
	const std::uint8_t *ptr = static_cast<const std::uint8_t *>(_surface->pixels) + y * _surface->pitch + x * bpp;
	col = GetPixelFromPointer(ptr);

	if (SDL_MUSTLOCK(_surface))
	{
		SDL_UnlockSurface(_surface);
	}

	return col;
}
