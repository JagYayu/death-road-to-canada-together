#pragma once

#include "Renderer.h"

#include "SDL3/SDL_render.h"
#include <SDL3/SDL.h>

#include <memory>

class SDL_Window;

namespace tudov
{
	class Engine;
	class Renderer;
	class Texture;

	class Window
	{
		friend Renderer;

	  private:
		SDL_Window *_window;

	  public:
		Engine &engine;
		Renderer renderer;

		Window(Engine &engine);

		SDL_Window *GetHandle();

		void Initialize();
		void PoolEvents();
		void Render();
	};
} // namespace tudov
