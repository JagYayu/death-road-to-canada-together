#pragma once

#include "Renderer.h"

#include "SDL3/SDL_render.h"
#include "debug/DebugManager.h"
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
		UInt32 _prevTick;
		UInt32 _frame;
		float _framerate;
		float framerateTimer;

	  public:
		Engine &engine;
		DebugManager debugManager;
		Renderer renderer;

		Window(Engine &engine);

		SDL_Window *GetHandle();

		float GetFramerate() const noexcept;

		void Initialize();
		void PoolEvents();
		void Render();
	};
} // namespace tudov
