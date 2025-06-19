#pragma once

#include "../IRenderBuffer.h"
#include "util/Log.h"

#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"

#include <string_view>

namespace tudov
{
	class SDLRenderer;

	class SDLRenderBuffer : public IRenderBuffer
	{
	  private:
		struct Command
		{
			SDL_Texture *tex;
			SDL_FRect dst;
			SDL_FRect src;
			float ang;
			SDL_FPoint ctr;
		};

	  private:
		std::shared_ptr<Log> _log;
		glm::mat3x3 _transform;
		std::vector<Command> _commandQueue;
		TTF_TextEngine *_textEngine;

	  public:
		SDLRenderBuffer(SDLRenderer &sdlRenderer) noexcept;

		glm::mat3x3 &GetTransform() noexcept override;
		void SetTransform(const glm::mat3x3 &transform) noexcept override;
		sol::object LuaGetTransform() noexcept override;
		void LuaSetTransform(const sol::table &mat3) noexcept override;
		void Clear() noexcept override;
		void Draw(std::float_t x, std::float_t y, std::float_t w, std::float_t h, ResourceID t, std::float_t tx, std::float_t ty, std::float_t tw, std::float_t th,
		          Color color = Color(255, 255, 255, 255), std::float_t a = 0, std::float_t cx = 0, std::float_t cy = 0, std::float_t z = 0) override;
		SDL_FRect DrawText(std::string_view text, const std::vector<FontID> &fonts, std::float_t x, std::float_t y, Color color = Color(255, 255, 255, 255),
		                   std::float_t scale = 1.f, std::float_t alignX = 0.f, std::float_t alignY = 0.f, std::float_t wrapWidth = -1, std::float_t shadow = 0) override;
		void Sort() noexcept override;
		void Render() noexcept override;
	};
} // namespace tudov