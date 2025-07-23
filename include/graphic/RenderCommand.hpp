#pragma once

#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"

#include <cmath>
#include <cstdint>

namespace tudov
{
	struct RenderCommand
	{
		enum class Type : std::uint8_t
		{
			DrawRect,
		};

		struct DrawRect
		{
			enum Nulls : std::uint8_t
			{
				Src = 1 << 0,
				Ori = 1 << 1,
			} nulls;
			SDL_Texture *tex;
			SDL_FRect dst;
			SDL_FRect src;
			SDL_FPoint ori;
			std::float_t ang;
		};

		Type type;
		union
		{
			DrawRect drawRect;
		} cmd;
		SDL_Color color;
		std::float_t z;

		bool operator<(const RenderCommand &other) const
		{
			return z < other.z;
		}

		bool operator>(const RenderCommand &other) const
		{
			return z > other.z;
		}
	};
} // namespace tudov