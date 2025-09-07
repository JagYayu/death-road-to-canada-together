/**
 * @file graphic/DrawArgs.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "sol/string_view.hpp"
#include "util/Color.hpp"
#include "util/Definitions.hpp"

#include <cmath>
#include <limits>
#include <string_view>

namespace tudov
{
	class LuaAPI;
	class Renderer;

	struct DrawTextArgs
	{
		friend LuaAPI;

		std::string_view text = "";
		FontID font = 0;
		std::float_t x = 0;
		std::float_t y = 0;
		std::float_t scale = 1;
		std::float_t characterScale = 1;
		std::float_t alignX = 0;
		std::float_t alignY = 0;
		std::float_t maxWidth = 0;
		Color color = Color();
		Color backgroundColor = Color(255, 0, 0, 0);
		std::float_t shadow = 0.0f;
		Color shadowColor = Color(0, 255, 0, 255);

	  private:
		inline std::string_view GetText() noexcept
		{
			return text;
		}
		inline std::double_t GetFont() noexcept
		{
			return font;
		}
		inline std::float_t GetCharacterScale() noexcept
		{
			return characterScale;
		}
		inline std::float_t GetX() noexcept
		{
			return x;
		}
		inline std::float_t GetY() noexcept
		{
			return y;
		}
		inline std::float_t GetScale() noexcept
		{
			return scale;
		}
		inline std::float_t GetAlignX() noexcept
		{
			return alignX;
		}
		inline std::float_t GetAlignY() noexcept
		{
			return alignY;
		}
		inline std::float_t GetMaxWidth() noexcept
		{
			return maxWidth;
		}

		inline void SetText(sol::string_view text) noexcept
		{
			this->text = text;
		}
		inline void SetFont(std::double_t font) noexcept
		{
			this->font = font;
		}
		inline void SetCharacterScale(std::float_t characterScale) noexcept
		{
			this->characterScale = characterScale;
		}
		inline void SetX(std::float_t x) noexcept
		{
			this->x = x;
		}
		inline void SetY(std::float_t y) noexcept
		{
			this->y = y;
		}
		inline void SetScale(std::float_t scale) noexcept
		{
			this->scale = scale;
		}
		inline void SetAlignX(std::float_t alignX) noexcept
		{
			this->alignX = alignX;
		}
		inline void SetAlignY(std::float_t alignY) noexcept
		{
			this->alignY = alignY;
		}
		inline void SetMaxWidth(std::float_t maxWidth) noexcept
		{
			this->maxWidth = maxWidth;
		}
	};

	// static constexpr decltype(auto) sizeOfDrawTextArgs = sizeof(DrawTextArgs);
} // namespace tudov
