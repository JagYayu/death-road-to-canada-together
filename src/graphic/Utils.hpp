#pragma once

#include "SDL3/SDL_pixels.h"
#include "bgfx/bgfx.h"

namespace tudov
{
	inline bgfx::TextureFormat::Enum SDLPixelFormat2BGFXTextureFormat(SDL_PixelFormat sdlFormat) noexcept
	{
		switch (sdlFormat)
		{
		// 8-bit formats
		case SDL_PIXELFORMAT_INDEX8:
			return bgfx::TextureFormat::R8;

		// 16-bit formats
		case SDL_PIXELFORMAT_RGB332:
			return bgfx::TextureFormat::R8; // Closest match
		case SDL_PIXELFORMAT_RGB565:
			return bgfx::TextureFormat::R5G6B5;
		case SDL_PIXELFORMAT_BGR565:
			return bgfx::TextureFormat::B5G6R5;
		case SDL_PIXELFORMAT_XRGB1555:
			return bgfx::TextureFormat::RGB5A1; // Closest match
		case SDL_PIXELFORMAT_ARGB4444:
			return bgfx::TextureFormat::RGBA4;
		case SDL_PIXELFORMAT_RGBA4444:
			return bgfx::TextureFormat::RGBA4;
		case SDL_PIXELFORMAT_ABGR4444:
			return bgfx::TextureFormat::RGBA4;
		case SDL_PIXELFORMAT_BGRA4444:
			return bgfx::TextureFormat::BGRA4;
		case SDL_PIXELFORMAT_ARGB1555:
			return bgfx::TextureFormat::RGB5A1;
		case SDL_PIXELFORMAT_RGBA5551:
			return bgfx::TextureFormat::RGB5A1;
		case SDL_PIXELFORMAT_ABGR1555:
			return bgfx::TextureFormat::RGB5A1;
		case SDL_PIXELFORMAT_BGRA5551:
			return bgfx::TextureFormat::BGR5A1;

		// 24-bit formats
		case SDL_PIXELFORMAT_RGB24:
			return bgfx::TextureFormat::RGB8;
		case SDL_PIXELFORMAT_BGR24:
			return bgfx::TextureFormat::RGB8; // No direct BGR8 in TextureFormat

		// 32-bit formats
		case SDL_PIXELFORMAT_RGBA8888:
			return bgfx::TextureFormat::RGBA8;
		case SDL_PIXELFORMAT_ARGB8888:
			return bgfx::TextureFormat::RGBA8;
		case SDL_PIXELFORMAT_ABGR8888:
			return bgfx::TextureFormat::RGBA8;
		case SDL_PIXELFORMAT_BGRA8888:
			return bgfx::TextureFormat::BGRA8;
		case SDL_PIXELFORMAT_RGBX8888:
			return bgfx::TextureFormat::RGBA8; // Alpha ignored
		case SDL_PIXELFORMAT_XRGB8888:
			return bgfx::TextureFormat::RGBA8; // Alpha ignored
		case SDL_PIXELFORMAT_XBGR8888:
			return bgfx::TextureFormat::RGBA8; // Alpha ignored
		case SDL_PIXELFORMAT_BGRX8888:
			return bgfx::TextureFormat::BGRA8; // Alpha ignored

		// YUV formats (no direct equivalent in TextureFormat)
		case SDL_PIXELFORMAT_YV12:
		case SDL_PIXELFORMAT_IYUV:
		case SDL_PIXELFORMAT_YUY2:
		case SDL_PIXELFORMAT_UYVY:
		case SDL_PIXELFORMAT_YVYU:
		case SDL_PIXELFORMAT_NV12:
		case SDL_PIXELFORMAT_NV21:
			return bgfx::TextureFormat::Unknown;

		default:
			return bgfx::TextureFormat::Unknown;
		}
	}
} // namespace tudov
