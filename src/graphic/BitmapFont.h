#pragma once

#include "SDL3/SDL_rect.h"
#include "graphic/IFont.h"

#include <cstdint>
#include <string_view>
#include <unordered_map>

namespace tudov
{
	/*
	 * Depreciated
	 */
	class BitmapFont : public IFont
	{
	  private:
		struct BoundingBox
		{
			std::int32_t minX;
			std::int32_t minY;
			std::int32_t maxX;
			std::int32_t maxY;
		};

		struct Item
		{
			std::int32_t id;
			std::int32_t minX;
			std::int32_t minY;
		};

	  private:
		TextureID _textureID;
		std::int32_t _width;
		std::int32_t _height;
		std::int32_t _channels;
		std::int32_t _glyphWidth;
		std::int32_t _glyphHeight;
		std::int32_t _columns;
		std::vector<unsigned char> _imageData;
		std::unordered_map<char32_t, SDL_Rect> _charMap;

	  public:
		explicit BitmapFont() noexcept = default;
		explicit BitmapFont(TextureID textureID, std::string_view fontData);
		~BitmapFont() noexcept = default;

		virtual TextureID GetTextureID() const noexcept;
		// virtual const std::unordered_map<char32_t, SDL_Rect> &GetCharMap() const noexcept override;
		virtual bool HasChar(char32_t ch) const override;
		virtual Char GetChar(char32_t ch, size_t sz) override;

		std::int32_t GetWidth() const noexcept;
		std::int32_t GetHeight() const noexcept;
		std::int32_t GetChannels() const noexcept;
		std::int32_t GetGlyphWidth() const noexcept;
		std::int32_t GetGlyphHeight() const noexcept;
		std::int32_t GetColumns() const noexcept;
	};
} // namespace tudov