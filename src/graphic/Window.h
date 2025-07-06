#pragma once

#include "Renderer.h"
#include "util/Log.h"

#include "SDL3/SDL_video.h"

#include <cmath>
#include <functional>
#include <memory>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace tudov
{
	class Engine;
	class ScriptEngine;

	class Window
	{
	  protected:
		std::shared_ptr<Log> _log;
		SDL_Window *_sdlWindow;
		bool _shouldClose;

	  public:
		Engine &engine;
		std::shared_ptr<Renderer> renderer;

	  public:
		explicit Window(Engine &engine, std::string_view logName = "Window") noexcept;
		~Window() noexcept;

	  protected:
		void HandleEvent(const SDL_Event &event) noexcept;
	  
	  public:
		virtual void Initialize(std::int32_t width, std::int32_t height, std::string_view title) noexcept;
		virtual void HandleEvents() noexcept;
		virtual void Render() noexcept;

		void Close() noexcept;
		bool ShouldClose() noexcept;

		SDL_Window *GetSDLWindowHandle() noexcept;
		std::int32_t GetWidth() const noexcept;
		std::int32_t GetHeight() const noexcept;
		std::tuple<std::int32_t, std::int32_t> GetSize() const noexcept;
		std::float_t GetFramerate() const noexcept;
	};
} // namespace tudov
