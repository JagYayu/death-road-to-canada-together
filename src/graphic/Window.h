#pragma once

#include <memory>
#include <SDL2/SDL.h>

class SDL_Window;

namespace tudov
{
	class GameEngine;

	class Window
	{
	private:
		GameEngine &_gameEngine;
		SDL_Window *_window;

	public:
		Window(GameEngine &engine);

		void Initialize();
		void PoolEvents();
	};
}
