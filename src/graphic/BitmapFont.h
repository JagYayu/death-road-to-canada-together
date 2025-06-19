#pragma once

#include "SDL3/SDL_rect.h"
#include "graphic/IFont.h"

#include <string_view>
#include <unordered_map>

namespace tudov
{
	class BitmapFont : public IFont
	{
	  private:
		std::int32_t _width;
		std::int32_t _height;
		std::int32_t _channels;
		std::int32_t _glyphWidth;
		std::int32_t _glyphHeight;
		std::int32_t _columns;
		std::vector<unsigned char> _imageData;
		std::unordered_map<char, SDL_Rect> _charMap;

	  public:
		explicit BitmapFont() noexcept = default;
		explicit BitmapFont(std::string_view fontData);
		~BitmapFont() noexcept = default;

		std::int32_t GetWidth() const noexcept;
		std::int32_t GetHeight() const noexcept;
		std::int32_t GetChannels() const noexcept;
		std::int32_t GetGlyphWidth() const noexcept;
		std::int32_t GetGlyphHeight() const noexcept;
		std::int32_t GetColumns() const noexcept;
		const std::unordered_map<char, SDL_Rect> &GetCharMap() const noexcept;
		bool HasChar(char c) const;
		SDL_Rect GetCharRect(char c) const;
	};
} // namespace tudov