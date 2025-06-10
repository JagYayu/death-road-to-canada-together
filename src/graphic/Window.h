#pragma once

#include <SDL2/SDL.h>
#include <memory>


class SDL_Window;

namespace tudov
{
	class Engine;

	class Window
	{
	  private:
		SDL_Window *_window;

	  public:
		Engine &engine;

		Window(Engine &engine);

		void Initialize();
		void PoolEvents();
	};
} // namespace tudov
