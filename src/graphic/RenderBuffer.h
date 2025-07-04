#pragma once

#include "SDL3/SDL_pixels.h"
#include "glm/ext/vector_float2.hpp"
#include "util/Defs.h"

#include "SDL3/SDL_rect.h"

namespace tudov
{
	class RenderBuffer
	{
	  public:
		struct Draw2DArgs
		{
			TextureID tex;
			SDL_Rect src;
			SDL_Rect dst;
			std::float_t z;
			SDL_Color color;
			std::float_t angle;
			glm::vec2 center;
		};

		struct DrawTextArgs
		{
			std::string_view text;
			std::vector<FontID> fonts;
			std::int32_t fontSize;
			std::float_t x;
			std::float_t y;
			std::float_t z;
			std::float_t scale;
			SDL_Color color;
			glm::vec2 align;
			std::float_t wrapWidth;
			std::float_t shadow;
			SDL_Color shadowColor;
		};

	  public:
		explicit RenderBuffer() noexcept = default;
	};
} // namespace tudov
