/**
 * @file graphic/Font.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "graphic/Font.hpp"

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "program/Tudov.hpp"

#include <span>
#include <string_view>

using namespace tudov;

Font::Font(std::string_view path, const std::vector<std::byte> &bytes) noexcept
    : _bytes(bytes.begin(), bytes.end())
{
}

Font::~Font() noexcept
{
}

#include <SDL3/SDL.h>
#include <algorithm>

#include <SDL3/SDL.h>
#include <algorithm>

SDL_Surface *CropTextSurface(SDL_Surface *src, std::int32_t padding = 1) noexcept
{
	std::uint32_t *pixels = static_cast<std::uint32_t *>(src->pixels);
	std::int32_t pitch = src->pitch / 4;

	std::int32_t minX = src->w;
	std::int32_t minY = src->h;
	std::int32_t maxX = -1;
	std::int32_t maxY = -1;

	const SDL_PixelFormatDetails *formatDetails = SDL_GetPixelFormatDetails(src->format);

	for (std::int32_t y = 0; y < src->h; ++y)
	{
		for (std::int32_t x = 0; x < src->w; ++x)
		{
			std::uint32_t pixel = pixels[y * pitch + x];
			std::uint8_t a;
			SDL_GetRGBA(pixel, formatDetails, nullptr, nullptr, nullptr, nullptr, &a);
			if (a > 0)
			{
				minX = std::min(minX, x);
				maxX = std::max(maxX, x);
				minY = std::min(minY, y);
				maxY = std::max(maxY, y);
			}
		}
	}

	if (maxX == -1)
	{
		return SDL_DuplicateSurface(src);
	}

	minX = std::max(0, minX - padding);
	minY = std::max(0, minY - padding);
	maxX = std::min(src->w - 1, maxX + padding);
	maxY = std::min(src->h - 1, maxY + padding);

	std::int32_t newW = maxX - minX + 1;
	std::int32_t newH = maxY - minY + 1;

	SDL_Rect srcRect{minX, minY, newW, newH};
	SDL_Surface *dst = SDL_CreateSurface(newW, newH, src->format);

	TE_ASSERT(dst);

	SDL_BlitSurface(src, &srcRect, dst, nullptr);

	return dst;
}

std::tuple<SDL_Texture *, std::int32_t, std::int32_t> Font::GetTextInfo(SDL_Renderer *sdlRenderer, std::uint16_t characterSize, std::string_view text, std::int32_t maxWidth) noexcept
{
	Key key{
	    sdlRenderer,
	    characterSize,
	    maxWidth,
	    std::string(text),
	};

	auto it = _lruMap.find(key);
	if (it != _lruMap.end()) [[likely]]
	{
		_lruList.splice(_lruList.begin(), _lruList, it->second);

		return std::make_tuple(it->second->texture, it->second->width, it->second->height);
	}

	auto itFont = _fonts.find(characterSize);
	if (itFont == _fonts.end()) [[unlikely]]
	{
		SDL_IOStream *stream = SDL_IOFromConstMem(_bytes.data(), _bytes.size_bytes());
		_fonts[characterSize] = TTF_OpenFontIO(stream, true, characterSize);
		itFont = _fonts.find(characterSize);
	}

	SDL_Texture *sdlTexture;
	std::int32_t width;
	std::int32_t height;
	{
		TTF_Font *font = itFont->second;

		SDL_Surface *sdlSurface = TTF_RenderText_Solid_Wrapped(font, text.data(), text.size(), SDL_Color{255, 255, 255, 255}, maxWidth);
		// sdlSurface = CropTextSurface(sdlSurface);

		sdlTexture = SDL_CreateTextureFromSurface(sdlRenderer, sdlSurface);
		TE_ASSERT(sdlTexture != nullptr, "{}", SDL_GetError());
		SDL_SetTextureScaleMode(sdlTexture, SDL_SCALEMODE_NEAREST);

		SDL_DestroySurface(sdlSurface);

		TTF_MeasureString(font, text.data(), text.size(), maxWidth, &width, nullptr);
		height = (maxWidth / width + 1) * TTF_GetFontHeight(font);
	}

	_lruList.push_front(Entry{
	    key,
	    sdlTexture,
	    width,
	    height,
	});
	_lruMap[key] = _lruList.begin();

	if (_lruMap.size() > 1024) [[unlikely]]
	{
		auto &last = _lruList.back();
		SDL_DestroyTexture(last.texture);

		_lruMap.erase(last.key);
		_lruList.pop_back();
	}

	return std::make_tuple(sdlTexture, width, height);
}
