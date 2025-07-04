#include "SDLTrueTypeFont.h"
#include "SDL3/SDL_iostream.h"

#include <format>
#include <stdexcept>

using namespace tudov;

SDLTrueTypeFont::SDLTrueTypeFont(std::string_view data)
    : _data(data)
{
	_defaultFontData = GetRaw(DefaultSize);
}

SDLTrueTypeFont::~SDLTrueTypeFont() noexcept
{
	for (auto &&[_, data] : _size2FontData)
	{
		TTF_CloseFont(data.raw);
	}
}

// void CreateNewPage()
//     {
//         SDL_Texture *tex = SDL_CreateTexture(SDLRenderer::Get(), SDL_PIXELFORMAT_RGBA32,
//                                              SDL_TEXTUREACCESS_TARGET, _pageWidth, _pageHeight);
//         if (!tex)
//             throw std::runtime_error(std::format("CreateTexture failed: {}", SDL_GetError()));

//         // 清空
//         SDL_SetRenderTarget(SDLRenderer::Get(), tex);
//         SDL_SetRenderDrawColor(SDLRenderer::Get(), 0, 0, 0, 0);
//         SDL_RenderClear(SDLRenderer::Get());
//         SDL_SetRenderTarget(SDLRenderer::Get(), nullptr);

//         _pages.push_back(tex);

//         _currentX = 0;
//         _currentY = 0;
//         _rowHeight = 0;
//     }

SDLTrueTypeFont::FontData &SDLTrueTypeFont::GetOrCreateFontData(size_t sz)
{
	{
		auto &&it = _size2FontData.find(sz);
		if (it != _size2FontData.end())
		{
			return it->second;
		}
	}

	auto stream = SDL_IOFromConstMem(_data.c_str(), _data.size());
	if (!stream)
	{
		throw std::runtime_error(std::format("SDL_IOFromConstMem failed: {}", SDL_GetError()));
	}

	auto font = TTF_OpenFontIO(stream, true, sz);
	if (!font)
	{
		throw std::runtime_error(std::format("TTF_OpenFontIO failed: {}", SDL_GetError()));
	}

	std::int32_t minX, maxX, minY, maxY;
	// TTF_GetGlyphMetrics(font, U'你', &minX, &maxX, &minY, &maxY, nullptr);

	return _size2FontData.try_emplace(sz, FontData{font, {}}).first->second;
}

TTF_Font *SDLTrueTypeFont::GetRaw(size_t sz)
{
	return GetOrCreateFontData(sz).raw;
}

// const std::unordered_map<char32_t, SDL_Rect> &SDLTrueTypeFont::GetCharMap() const noexcept
// {
// 	return _charMap;
// }

// TextureID SDLTrueTypeFont::GetTextureID() const noexcept
// {
// }

bool SDLTrueTypeFont::HasChar(char32_t ch) const
{
	return TTF_FontHasGlyph(_defaultFontData, ch);
}

IFont::Char SDLTrueTypeFont::GetChar(char32_t ch, size_t sz)
{
	std::int32_t minX, maxX, minY, maxY;
	TTF_GetGlyphMetrics(_defaultFontData, ch, &minX, &maxX, &minY, &maxY, nullptr);

	return {
	    .tex = false, // TODO
	    .rect = GetOrCreateFontData(sz).charMap.at(ch).rect,
	};
}
