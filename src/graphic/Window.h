#pragma once

#include "SDL3/SDL_render.h"
#include <SDL3/SDL.h>

#include <memory>

class SDL_Window;

namespace tudov
{
	class Engine;

	class Window
	{
	  private:
		SDL_Window *_window;
		SDL_Renderer *_renderer;

	  public:
		Engine &engine;

		Window(Engine &engine);

		SDL_Window *GetHandle();

		void Initialize();
		void PoolEvents();
		void Render();
	};
} // namespace tudov
