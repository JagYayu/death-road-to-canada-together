#pragma once

#include "DynamicBuffer.h"
#include "ErrorOverlay.h"
#include "Renderer.h"
#include "debug/DebugManager.h"
#include "mod/ScriptEngine.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_render.h"


#include <cmath>
#include <cstddef>
#include <memory>
#include <tuple>

typedef struct SDL_GLContextState *SDL_GLContext;
class SDL_Window;

namespace tudov
{
	class Engine;
	struct EventHandleKey;
	class SDLRenderer;
	class Texture2D;

	class Window
	{
		friend SDLRenderer;

	  private:
		SDL_Window *_window;
		SDL_Renderer *_renderer;
		bool _initialized;
		std::uint32_t _prevTick;
		std::uint32_t _frame;
		std::double_t _framerate;
		ErrorOverlay _errorOverlay;

	  public:
		Engine &engine;
		Renderer renderer;
		DebugManager debugManager;

	  public:
		Window(Engine &engine);
		~Window() noexcept;

	  private:
		std::tuple<sol::table, EventHandleKey> ResolveKeyEvent(SDL_Event &event) noexcept;
		std::tuple<sol::table, EventHandleKey> ResolveMouseButtonEvent(SDL_Event &event) noexcept;

	  public:
		SDL_Window *GetHandle();

		float GetFramerate() const noexcept;
		std::tuple<std::int32_t, std::int32_t> GetSize() const noexcept;

		void Initialize();
		void Deinitialize() noexcept;
		void InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept;
		void PollEvents();
		void Render();
	};
} // namespace tudov
