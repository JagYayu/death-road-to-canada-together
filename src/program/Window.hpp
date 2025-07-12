#pragma once

#include "EngineComponent.hpp"
#include "graphic/Renderer.hpp"
#include "util/Log.hpp"

#include <memory>
#include <string_view>
#include <tuple>

struct SDL_Window;

namespace tudov
{
	class Engine;
	class EventHandleKey;
	class ScriptEngine;

	struct IWindow : public IContextProvider
	{
		virtual void Initialize(std::int32_t width, std::int32_t height, std::string_view title) noexcept = 0;
		virtual std::int32_t GetWidth() const noexcept = 0;
		virtual std::int32_t GetHeight() const noexcept = 0;
		virtual std::float_t GetDisplayScale() const noexcept = 0;
		virtual std::tuple<std::int32_t, std::int32_t> GetSize() const noexcept = 0;
		virtual void Close() noexcept = 0;
		virtual bool ShouldClose() noexcept = 0;
		virtual void HandleEvents() noexcept = 0;
		virtual bool HandleEvent(SDL_Event &event) noexcept = 0;
		virtual void Render() noexcept = 0;
	};

	class Window : public IWindow
	{
	  protected:
		Context &_context;
		std::shared_ptr<Log> _log;
		SDL_Window *_sdlWindow;
		bool _shouldClose;

	  public:
		std::shared_ptr<Renderer> renderer;

	  public:
		explicit Window(Context &context, std::string_view logName = "Window") noexcept;
		~Window() noexcept override;

		protected:
		void RenderPreImpl() noexcept;

	  public:
		Context &GetContext() noexcept override;
		void Initialize(std::int32_t width, std::int32_t height, std::string_view title) noexcept override;
		std::int32_t GetWidth() const noexcept override;
		std::int32_t GetHeight() const noexcept override;
		std::tuple<std::int32_t, std::int32_t> GetSize() const noexcept override;
		std::float_t GetDisplayScale() const noexcept override;
		void Close() noexcept override;
		bool ShouldClose() noexcept override;
		void HandleEvents() noexcept override;
		bool HandleEvent(SDL_Event &event) noexcept override;
		void Render() noexcept override;

		virtual EventHandleKey GetKey() const noexcept;

		SDL_Window *GetSDLWindowHandle() noexcept;
	};
} // namespace tudov
