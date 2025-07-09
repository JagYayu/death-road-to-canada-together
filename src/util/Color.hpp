#pragma once

#include "SDL3/SDL_pixels.h"

#include <cstdint>

namespace tudov
{
	class Color
	{
	  private:
		static constexpr decltype(auto) SHIFT_R = 24;
		static constexpr decltype(auto) SHIFT_G = 16;
		static constexpr decltype(auto) SHIFT_B = 8;
		static constexpr decltype(auto) SHIFT_A = 0;

	  public:
		std::uint8_t r;
		std::uint8_t g;
		std::uint8_t b;
		std::uint8_t a;

	  public:
		explicit constexpr Color(std::uint8_t r = 0, std::uint8_t g = 0, std::uint8_t b = 0, std::uint8_t a = 255) noexcept
		    : r(r), g(g), b(b), a(a)
		{
		}

		explicit constexpr Color(std::uint32_t value) noexcept
		    : r((value >> SHIFT_R) & 0xFF), g((value >> SHIFT_G) & 0xFF), b((value >> SHIFT_B) & 0xFF), a((value >> SHIFT_A) & 0xFF)
		{
		}

		inline constexpr bool operator==(const Color &other) const noexcept
		{
			return r == other.r && g == other.g && b == other.b && a == other.a;
		}
		inline constexpr bool operator!=(const Color &other) const noexcept
		{
			return r != other.r || g != other.g || b != other.b | a == other.a;
		}

		inline operator SDL_Color() const noexcept
		{
			return SDL_Color{.r = r, .g = g, .b = b, .a = a};
		}
	};
} // namespace tudov
