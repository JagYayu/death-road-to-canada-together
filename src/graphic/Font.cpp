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

SDL_Texture *Font::GetTextTexture(SDL_Renderer *sdlRenderer, std::uint16_t characterSize, std::string_view text, std::int32_t maxWidth) noexcept
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

		return it->second->texture;
	}

	auto itFont = _fonts.find(characterSize);
	if (itFont == _fonts.end()) [[unlikely]]
	{
		SDL_IOStream *stream = SDL_IOFromConstMem(_bytes.data(), _bytes.size_bytes());
		_fonts[characterSize] = TTF_OpenFontIO(stream, true, characterSize);
		itFont = _fonts.find(characterSize);
	}

	SDL_Surface *sdlSurface = TTF_RenderText_Solid_Wrapped(itFont->second, text.data(), text.size(), SDL_Color{255, 255, 255, 255}, maxWidth);
	SDL_Texture *sdlTexture = SDL_CreateTextureFromSurface(sdlRenderer, sdlSurface);
	TE_ASSERT(sdlTexture != nullptr, "{}", SDL_GetError());
	SDL_SetTextureScaleMode(sdlTexture, SDL_SCALEMODE_NEAREST);
	SDL_DestroySurface(sdlSurface);

	_lruList.push_front(Entry{key, sdlTexture});
	_lruMap[key] = _lruList.begin();

	if (_lruMap.size() > 1024) [[unlikely]]
	{
		auto &last = _lruList.back();
		SDL_DestroyTexture(last.texture);

		_lruMap.erase(last.key);
		_lruList.pop_back();
	}

	return sdlTexture;

	// Key key{
	//     sdlRenderer,
	//     characterSize,
	//     maxWidth,
	//     text.data(),
	// };

	// auto it = _lruMap.find(key);
	// if (it == _lruMap.end()) [[unlikely]]
	// {
	// 	auto itFont = _fonts.find(characterSize);
	// 	if (itFont == _fonts.end()) [[unlikely]]
	// 	{
	// 		SDL_IOStream *stream = SDL_IOFromConstMem(_bytes.data(), _bytes.size_bytes());

	// 		_fonts[characterSize] = TTF_OpenFontIO(stream, true, characterSize);

	// 		itFont = _fonts.find(characterSize);
	// 	}

	// 	SDL_Surface *sdlSurface = TTF_RenderText_Solid_Wrapped(itFont->second, text.data(), text.size(), SDL_Color(255, 255, 255, 255), maxWidth);
	// 	SDL_Texture *sdlTexture = SDL_CreateTextureFromSurface(sdlRenderer, sdlSurface);
	// 	SDL_SetTextureScaleMode(sdlTexture, SDL_SCALEMODE_NEAREST);
	// 	SDL_DestroySurface(sdlSurface);

	// 	_lruMap[key] = sdlTexture;

	// 	it = _lruMap.find(key);
	// }

	// return it->second;
}
