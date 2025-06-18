#pragma once

#include "ErrorOverlay.h"
#include "IRenderer.h"

#include "SDL3/SDL_render.h"
#include "debug/DebugManager.h"
#include "mod/ScriptEngine.h"
#include <SDL3/SDL.h>

#include <cmath>
#include <memory>
#include <tuple>

class SDL_Window;

namespace tudov
{
	class Engine;
	struct EventHandleKey;
	class SDLRenderer;
	class Texture;

	class Window
	{
		friend SDLRenderer;

	  private:
		SDL_Window *_window;
		std::uint32_t _prevTick;
		std::uint32_t _frame;
		std::double_t _framerate;
		ErrorOverlay _errorOverlay;

	  public:
		Engine &engine;
		DebugManager debugManager;
		std::unique_ptr<IRenderer> renderer;

	  public:
		Window(Engine &engine);
		~Window() noexcept;

	  private:
		std::tuple<sol::table, EventHandleKey> ResolveKeyEvent(SDL_Event& event) noexcept;
		std::tuple<sol::table, EventHandleKey> ResolveMouseButtonEvent(SDL_Event &event) noexcept;

	  public:
		SDL_Window *GetHandle();

		float GetFramerate() const noexcept;

		void Initialize();
		void Deinitialize() noexcept;
		void InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept;
		void PoolEvents();
		void Render();
	};
} // namespace tudov
