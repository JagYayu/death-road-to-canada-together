#pragma once

#include "../IRenderBuffer.h"
#include "util/Log.h"

#include "SDL3/SDL_render.h"

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

	  public:
		SDLRenderBuffer(SDLRenderer &sdlRenderer) noexcept;

		glm::mat3x3 &GetTransform() noexcept;
		void SetTransform(const glm::mat3x3 &transform) noexcept;
		sol::object LuaGetTransform() noexcept;
		void LuaSetTransform(const sol::table &mat3) noexcept;
		void Clear() noexcept;
		void Draw(float x, float y, float w, float h, ResourceID t, float tx, float ty, float tw, float th, float a, float cx, float cy, float z) noexcept;
		void Sort() noexcept;
		void Render() noexcept;
	};
} // namespace tudov