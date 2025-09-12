/**
 * @file graphic/RenderArgs.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "math/Geometry.hpp"
#include "util/Color.hpp"
#include "util/Definitions.hpp"

#include "sol/string_view.hpp"
#include "sol/table.hpp"

#include <cmath>
#include <memory>
#include <optional>
#include <string_view>

namespace tudov
{
	class LuaAPI;
	class RenderTarget;
	class Renderer;
	class Texture;

	struct DrawRectArgs
	{
		friend LuaAPI;
		friend Renderer;

		sol::object texture = sol::nil;
		RectangleF destination = RectangleF();
		std::optional<RectangleF> source = std::nullopt;
		Color color = Color::White;
		std::float_t angle = 0;
		std::float_t originX = 0;
		std::float_t originY = 0;

	  private:
		std::optional<std::shared_ptr<Texture>> _texture = std::nullopt;

	  private:
		inline sol::object LuaGetTexture() noexcept
		{
			return texture;
		}
		inline RectangleF *LuaGetDestination() noexcept
		{
			return &destination;
		}
		inline RectangleF *LuaGetSource() noexcept
		{
			if (source.has_value())
			{
				return &source.value();
			}
			else
			{
				return nullptr;
			}
		}
		inline std::uint32_t LuaGetColor() noexcept
		{
			return static_cast<std::uint32_t>(color);
		}
		inline std::float_t LuaGetAngle() noexcept
		{
			return angle;
		}
		inline std::float_t LuaGetOriginX() noexcept
		{
			return originX;
		}
		inline std::float_t LuaGetOriginY() noexcept
		{
			return originY;
		}

		inline void LuaSetTexture(sol::object texture) noexcept
		{
			this->texture = texture;
			_texture = std::nullopt;
		}
		inline void LuaSetDestination(sol::table destination) noexcept
		{
			this->destination = RectangleF{
			    destination.get_or("x", 0.0f),
			    destination.get_or("y", 0.0f),
			    destination.get_or("w", 0.0f),
			    destination.get_or("h", 0.0f),
			};
		}
		inline void LuaSetSource(sol::object source) noexcept
		{
			if (source.is<sol::table>())
			{
				auto source1 = source.as<sol::table>();
				this->source = RectangleF{
				    source1.get_or("x", 0.0f),
				    source1.get_or("y", 0.0f),
				    source1.get_or("w", 0.0f),
				    source1.get_or("h", 0.0f),
				};
			}
			else
			{
				this->source = std::nullopt;
			}
		}
		inline void LuaSetColor(std::double_t color) noexcept
		{
			this->color = Color(static_cast<std::uint32_t>(color));
		}
		inline void LuaSetAngle(std::double_t angle) noexcept
		{
			this->angle = angle;
		}
		inline void LuaSetOriginX(std::double_t originX) noexcept
		{
			this->originX = originX;
		}
		inline void LuaSetOriginY(std::double_t originY) noexcept
		{
			this->originY = originY;
		}
	};

	struct DrawTextArgs
	{
		friend LuaAPI;

		FontID font = 0.0f;
		std::string_view text = "";
		std::float_t x = 0.0f;
		std::float_t y = 0.0f;
		std::float_t scale = 1.0f;
		std::float_t characterSize = 20.0f;
		std::float_t alignX = 0.0f;
		std::float_t alignY = 0.0f;
		std::float_t maxWidth = 0.0f;
		Color color = Color();
		Color backgroundColor = Color(255, 0, 0, 0);
		std::float_t shadow = 0.0f;
		Color shadowColor = Color(0, 0, 0, 255);

	  private:
		inline std::string_view GetText() noexcept
		{
			return text;
		}
		inline std::double_t GetFont() noexcept
		{
			return font;
		}
		inline std::float_t GetCharacterSize() noexcept
		{
			return characterSize;
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
		inline std::uint32_t GetColor() noexcept
		{
			return static_cast<std::uint32_t>(color);
		}
		inline std::uint32_t GetBackgroundColor() noexcept
		{
			return static_cast<std::uint32_t>(backgroundColor);
		}
		inline std::float_t GetShadow() noexcept
		{
			return shadow;
		}
		inline std::uint32_t GetShadowColor() noexcept
		{
			return static_cast<std::uint32_t>(shadowColor);
		}

		inline void SetText(sol::string_view text) noexcept
		{
			this->text = text;
		}
		inline void SetFont(std::double_t font) noexcept
		{
			this->font = font;
		}
		inline void SetCharacterSize(std::float_t characterSize) noexcept
		{
			this->characterSize = characterSize;
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
		inline void SetColor(std::uint32_t color) noexcept
		{
			this->color = Color(color);
		}
		inline void SetBackgroundColor(std::uint32_t backgroundColor) noexcept
		{
			this->backgroundColor = Color(backgroundColor);
		}
		inline void SetShadow(std::float_t shadow) noexcept
		{
			this->shadow = shadow;
		}
		inline void SetShadowColor(std::uint32_t color) noexcept
		{
			this->shadowColor = Color(shadowColor);
		}
	};
} // namespace tudov
