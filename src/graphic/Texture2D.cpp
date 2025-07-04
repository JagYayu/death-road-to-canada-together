#include "Texture2D.h"

#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_surface.h"
#include "Utils.hpp"
#include "bgfx/bgfx.h"

#include <cstdint>
#include <tuple>

using namespace tudov;

std::int32_t Texture2D::GetTextureMemorySizeFromSurface(const SDL_Surface &surface) noexcept
{
	std::int32_t bytesPerPixel = 0;
	switch (surface.format)
	{
	case SDL_PIXELFORMAT_INDEX1LSB:
	case SDL_PIXELFORMAT_INDEX1MSB:
		bytesPerPixel = 0;
		break;
	case SDL_PIXELFORMAT_INDEX2LSB:
	case SDL_PIXELFORMAT_INDEX2MSB:
		bytesPerPixel = 0;
		break;
	case SDL_PIXELFORMAT_INDEX4LSB:
	case SDL_PIXELFORMAT_INDEX4MSB:
		bytesPerPixel = 0;
		break;
	case SDL_PIXELFORMAT_INDEX8:
		bytesPerPixel = 1;
		break;
	case SDL_PIXELFORMAT_RGB332:
		bytesPerPixel = 1;
		break;
	case SDL_PIXELFORMAT_XRGB4444:
	case SDL_PIXELFORMAT_XBGR4444:
	case SDL_PIXELFORMAT_XRGB1555:
	case SDL_PIXELFORMAT_XBGR1555:
	case SDL_PIXELFORMAT_ARGB4444:
	case SDL_PIXELFORMAT_RGBA4444:
	case SDL_PIXELFORMAT_ABGR4444:
	case SDL_PIXELFORMAT_BGRA4444:
	case SDL_PIXELFORMAT_ARGB1555:
	case SDL_PIXELFORMAT_RGBA5551:
	case SDL_PIXELFORMAT_ABGR1555:
	case SDL_PIXELFORMAT_BGRA5551:
	case SDL_PIXELFORMAT_RGB565:
	case SDL_PIXELFORMAT_BGR565:
		bytesPerPixel = 2;
		break;
	case SDL_PIXELFORMAT_RGB24:
	case SDL_PIXELFORMAT_BGR24:
		bytesPerPixel = 3;
		break;
	case SDL_PIXELFORMAT_XRGB8888:
	case SDL_PIXELFORMAT_RGBX8888:
	case SDL_PIXELFORMAT_XBGR8888:
	case SDL_PIXELFORMAT_BGRX8888:
	case SDL_PIXELFORMAT_ARGB8888:
	case SDL_PIXELFORMAT_RGBA8888:
	case SDL_PIXELFORMAT_ABGR8888:
	case SDL_PIXELFORMAT_BGRA8888:
	case SDL_PIXELFORMAT_XRGB2101010:
	case SDL_PIXELFORMAT_XBGR2101010:
	case SDL_PIXELFORMAT_ARGB2101010:
	case SDL_PIXELFORMAT_ABGR2101010:
		bytesPerPixel = 4;
		break;
	case SDL_PIXELFORMAT_RGB48:
	case SDL_PIXELFORMAT_BGR48:
	case SDL_PIXELFORMAT_RGB48_FLOAT:
	case SDL_PIXELFORMAT_BGR48_FLOAT:
		bytesPerPixel = 6;
		break;
	case SDL_PIXELFORMAT_RGBA64:
	case SDL_PIXELFORMAT_ARGB64:
	case SDL_PIXELFORMAT_BGRA64:
	case SDL_PIXELFORMAT_ABGR64:
	case SDL_PIXELFORMAT_RGBA64_FLOAT:
	case SDL_PIXELFORMAT_ARGB64_FLOAT:
	case SDL_PIXELFORMAT_BGRA64_FLOAT:
	case SDL_PIXELFORMAT_ABGR64_FLOAT:
		bytesPerPixel = 8;
		break;
	case SDL_PIXELFORMAT_RGB96_FLOAT:
	case SDL_PIXELFORMAT_BGR96_FLOAT:
		bytesPerPixel = 12;
		break;
	case SDL_PIXELFORMAT_RGBA128_FLOAT:
	case SDL_PIXELFORMAT_ARGB128_FLOAT:
	case SDL_PIXELFORMAT_BGRA128_FLOAT:
	case SDL_PIXELFORMAT_ABGR128_FLOAT:
		bytesPerPixel = 16;
		break;
	case SDL_PIXELFORMAT_YV12:
	case SDL_PIXELFORMAT_IYUV:
	case SDL_PIXELFORMAT_YUY2:
	case SDL_PIXELFORMAT_UYVY:
	case SDL_PIXELFORMAT_YVYU:
	case SDL_PIXELFORMAT_NV12:
	case SDL_PIXELFORMAT_NV21:
	case SDL_PIXELFORMAT_P010:
	case SDL_PIXELFORMAT_EXTERNAL_OES:
	case SDL_PIXELFORMAT_MJPG:
		bytesPerPixel = 0;
		break;
	default:
		_log->Error("Unsupported pixel format: {}", (std::int32_t)surface.format);
		break;
	}

	std::int32_t totalSize = 0;
	if (bytesPerPixel > 0)
	{
		totalSize = surface.w * surface.h * bytesPerPixel;
	}
	else
	{
		switch (surface.format)
		{
		case SDL_PIXELFORMAT_INDEX1LSB:
		case SDL_PIXELFORMAT_INDEX1MSB:
			totalSize = ((surface.w + 7) / 8) * surface.h;
			break;
		case SDL_PIXELFORMAT_INDEX2LSB:
		case SDL_PIXELFORMAT_INDEX2MSB:
			totalSize = ((surface.w + 3) / 4) * surface.h;
			break;
		case SDL_PIXELFORMAT_INDEX4LSB:
		case SDL_PIXELFORMAT_INDEX4MSB:
			totalSize = ((surface.w + 1) / 2) * surface.h;
			break;
		case SDL_PIXELFORMAT_YV12:
		case SDL_PIXELFORMAT_IYUV:
			totalSize = surface.w * surface.h + 2 * ((surface.w + 1) / 2) * ((surface.h + 1) / 2);
			break;
		default:
			_log->Error("Compressed/YUV formats require manual size calculation");
			break;
		}
	}

	return totalSize;
}

Texture2D::Texture2D(const SDL_Surface &surface)
    : _log(Log::Get("Texture2D")),
      _width(surface.w),
      _height(surface.h)
{
	const bgfx::Memory *mem = bgfx::copy(surface.pixels, GetTextureMemorySizeFromSurface(surface));
	_textureHandle = bgfx::createTexture2D(surface.w, surface.h, false, 1, SDLPixelFormat2BGFXTextureFormat(surface.format), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, mem);
}

Texture2D::~Texture2D() noexcept
{
}

bgfx::TextureHandle Texture2D::GetHandle() noexcept
{
	return _textureHandle;
}

std::uint32_t Texture2D::GetWidth() const noexcept
{
	return _width;
}

std::uint32_t Texture2D::GetHeight() const noexcept
{
	return _height;
}

std::tuple<std::uint32_t, std::uint32_t> Texture2D::GetSize() const noexcept
{
	return {_width, _height};
}
