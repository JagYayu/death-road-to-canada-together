#pragma once

#include "util/Defs.h"

#include "SDL3/SDL_rect.h"

namespace tudov
{
	struct IFont
	{
		struct Char
		{
			TextureID tex;
			SDL_Rect rect;
		};

		inline static constexpr size_t DefaultSize = 16;

		// virtual TextureID GetTextureID() const noexcept = 0;
		// virtual const std::unordered_map<char32_t, SDL_Rect> &GetCharMap() const noexcept = 0;
		virtual bool HasChar(char32_t ch) const = 0;
		virtual Char GetChar(char32_t ch, size_t sz = DefaultSize) = 0;
	};
} // namespace tudov
