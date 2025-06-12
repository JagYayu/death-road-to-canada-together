#pragma once

#include "resource/ResourceManager.hpp"
#include "util/Defs.h"

#include "SDL3/SDL_render.h"

namespace tudov
{
	class Texture;
	class Window;

	class Renderer
	{
		friend Texture;
		
	  private:
		struct Draw
		{
			SDL_Texture *tex;
			SDL_FRect dst;
			SDL_FRect src;
		};

		Window &window;
		SDL_Renderer *_renderer;

		Vector<Draw> _drawQueue;

	  public:
		Renderer(Window &window);

		void Initialize()noexcept;

		void DrawSprite(ResourceID texID, Number x, Number y, Number w, Number h, Number tw, Number th);

		void Render();
	};
} // namespace tudov