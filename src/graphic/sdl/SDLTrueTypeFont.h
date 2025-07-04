#pragma once

#include "graphic/IFont.h"

#include <SDL3_ttf/SDL_ttf.h>

#include <string>
#include <string_view>
#include <unordered_map>

namespace tudov
{
	class SDLTrueTypeFont : public IFont
	{
	  private:
		struct GlyphInfo
		{
			int page;
			SDL_Rect rect;
			int advance;
			int offsetX, offsetY;
		};

		struct FontData
		{
			TTF_Font *raw;
			std::unordered_map<char32_t, GlyphInfo> charMap;
		};

		std::string _data;
		TTF_Font *_defaultFontData;
		std::unordered_map<size_t, FontData> _size2FontData;

		std::vector<SDL_Texture *> _pages;

		int _pageWidth = 2048;
		int _pageHeight = 2048;
		int _padding = 2;

		int _currentX = 0;
		int _currentY = 0;
		int _rowHeight = 0;

	  public:
		explicit SDLTrueTypeFont(std::string_view data);
		~SDLTrueTypeFont() noexcept;

	  protected:
		void CreateNewPage();
		SDLTrueTypeFont::FontData &GetOrCreateFontData(size_t sz);

	  public:
		TTF_Font *GetRaw(std::size_t size);

		virtual bool HasChar(char32_t ch) const override;
		virtual Char GetChar(char32_t ch, size_t sz) override;
	};
} // namespace tudov
