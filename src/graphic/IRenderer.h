#pragma once

#include "IRenderBackend.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "glm/ext/matrix_float3x3.hpp"
#include "sol/table.hpp"
#include "util/Defs.h"

#include <memory>
#include <string_view>

#undef DrawText

namespace tudov
{
	class ScriptEngine;
	class Window;

	struct IRenderer
	{
		Window &window;

		IRenderer(Window &window) noexcept;

		struct DrawArgs
		{
			ResourceID tex;
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

		virtual void Initialize() noexcept = 0;
		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual glm::mat3x3 GetTransform() = 0;
		virtual void SetTransform(const glm::mat3x3 &mat3) = 0;
		virtual void Clear() = 0;
		virtual void Draw(const DrawArgs &args) = 0;
		virtual SDL_FRect DrawText(const DrawTextArgs &args) = 0;
		virtual void Sort() = 0;
		virtual void Render() = 0;
		virtual sol::object LuaGetTransform() = 0;
		virtual void LuaSetTransform(const sol::table &mat3) = 0;

		void LuaDraw(const sol::table &args) noexcept;
		SDL_FRect LuaDrawText(const sol::table &args) noexcept;
	};
} // namespace tudov
