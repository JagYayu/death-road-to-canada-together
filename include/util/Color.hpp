/**
 * @file util/Color.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <cstdint>

struct SDL_Color;
struct SDL_FColor;

namespace tudov
{
	class Color
	{
	  public:
		static const Color White;
		static const Color Red;
		static const Color Green;
		static const Color Blue;
		static const Color Black;
		static const Color Transparent;

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
		inline explicit constexpr Color() noexcept
		    : Color(255, 255, 255, 255)
		{
		}

		inline explicit constexpr Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) noexcept
		    : r(r), g(g), b(b), a(a)
		{
		}

		inline explicit constexpr Color(std::uint32_t value) noexcept
		    : r((value >> SHIFT_R) & 0xFF),
		      g((value >> SHIFT_G) & 0xFF),
		      b((value >> SHIFT_B) & 0xFF),
		      a((value >> SHIFT_A) & 0xFF)
		{
		}

		Color(const Color &) noexcept = default;
		explicit Color(Color &&) noexcept = default;
		Color &operator=(const Color &) noexcept = default;
		Color &operator=(Color &&) noexcept = default;
		~Color() noexcept = default;

		inline constexpr bool operator==(const Color &other) const noexcept
		{
			return r == other.r && g == other.g && b == other.b && a == other.a;
		}

		inline constexpr bool operator!=(const Color &other) const noexcept
		{
			return r != other.r || g != other.g || b != other.b || a == other.a;
		}

		inline explicit constexpr operator std::uint32_t() const noexcept
		{
			return static_cast<std::uint32_t>(r) << SHIFT_R |
			       static_cast<std::uint32_t>(g) << SHIFT_G |
			       static_cast<std::uint32_t>(b) << SHIFT_B |
			       static_cast<std::uint32_t>(a) << SHIFT_A;
		}

		operator SDL_Color() const noexcept;
		operator SDL_FColor() const noexcept;
	};
} // namespace tudov
