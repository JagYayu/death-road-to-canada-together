#pragma once

#include "../IRenderer.h"
#include "SDL3/SDL_surface.h"
#include "resource/ResourceManager.hpp"
#include "util/Defs.h"

#include "SDL3/SDL_render.h"
#include "util/Log.h"

namespace tudov
{
	class ScriptEngine;
	class Texture;
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

		// std::vector<Command> _commandQueue;

	  public:
		SDLRenderer(Window &window);

		SDL_Renderer *GetRaw() noexcept;
		const SDL_Renderer *GetRaw() const noexcept;

		void Initialize() noexcept override;

		void InstallToScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept override;

		// void Draw(ResourceID texID, float x, float y, float w, float h, float tx, float ty, float tw, float th, float ang, float cx, float cy, uint32_t flip);

		void Begin() override;
		void End() override;
	};
} // namespace tudov