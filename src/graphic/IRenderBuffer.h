#pragma once

#include "ITexture.h"
#include "util/Defs.h"

#include "SDL3/SDL_rect.h"
#include "glm/glm.hpp"
#include "sol/table.hpp"

#include <array>

namespace tudov
{
	class IRenderer;

	struct IRenderBuffer
	{
		IRenderer &renderer;

		IRenderBuffer(IRenderer &renderer) noexcept;

		virtual glm::mat3x3 &GetTransform() = 0;
		virtual void SetTransform(const glm::mat3x3 &mat3) = 0;
		virtual void Clear() = 0;
		virtual void Draw(std::float_t x, std::float_t y, std::float_t w, std::float_t h, ResourceID t, std::float_t tx, std::float_t ty, std::float_t tw, std::float_t th,
		                  Color color = Color(255, 255, 255, 255), std::float_t a = 0, std::float_t cx = 0, std::float_t cy = 0, std::float_t z = 0) = 0;
		virtual SDL_FRect DrawText(std::string_view text,const std::vector<FontID> &fonts, std::float_t x, std::float_t y, Color color = Color(255, 255, 255, 255),
		                           std::float_t scale = 1.f, std::float_t alignX = 0.f, std::float_t alignY = 0.f, std::float_t wrapWidth = -1, std::float_t shadow = 0) = 0;
		virtual void Sort() = 0;
		virtual void Render() = 0;

		virtual sol::object LuaGetTransform() = 0;
		virtual void LuaSetTransform(const sol::table &mat3) = 0;
		void LuaDraw(std::float_t x, std::float_t y, std::float_t w, std::float_t h, ResourceID t, std::float_t tx, std::float_t ty, std::float_t tw, std::float_t th,
		             std::uint32_t color = -1, std::float_t a = 0, std::float_t cx = 0, std::float_t cy = 0, std::float_t z = 0) noexcept;
		SDL_FRect LuaDrawText(sol::string_view text, const sol::table &fonts, std::float_t x, std::float_t y, std::uint32_t color = -1,
		                      std::float_t scale = 1.f, std::float_t alignX = 0.f, std::float_t alignY = 0.f, std::float_t wrapWidth = -1, std::float_t shadow = 0) noexcept;
	};
} // namespace tudov
