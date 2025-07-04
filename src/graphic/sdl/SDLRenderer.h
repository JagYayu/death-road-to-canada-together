#pragma once

#include "../IRenderer.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "graphic/ERenderBackend.h"
#include "resource/ResourceManager.hpp"
#include "util/Defs.h"

#include "SDL3/SDL_render.h"
#include "util/Log.h"

#undef DrawText

namespace tudov
{
	class ScriptEngine;
	class Texture2D;
	class Window;

	class SDLRenderer : public IRenderer
	{
	  private:
		struct Command
		{
			SDL_Texture *tex;
			SDL_FRect dst;
			SDL_FRect src;
			float ang;
			SDL_FPoint ctr;
			SDL_FlipMode flip;
		};

	  private:
		std::shared_ptr<Log> _log;
		SDL_Renderer *_renderer;
		TTF_TextEngine *_textEngine;

		// std::vector<Command> _commandQueue;

	  public:
		SDLRenderer(Window &window);

	  public:
		SDL_Renderer *GetRaw() noexcept;
		const SDL_Renderer *GetRaw() const noexcept;
		TTF_TextEngine *GetRawTextEngine() noexcept;
		const TTF_TextEngine *GetRawTextEngine() const noexcept;

		void Initialize() noexcept override;
		void Begin() override;
		void End() override;
		glm::mat3x3 GetTransform() override;
		void SetTransform(const glm::mat3x3 &mat3) override;
		void Clear() override;
		void Draw(const DrawArgs &args) override;
		SDL_FRect DrawText(const DrawTextArgs &args) override;
		void Sort() override;
		void Render() override;
		sol::object LuaGetTransform() override;
		void LuaSetTransform(const sol::table &mat3) override;
	};
} // namespace tudov