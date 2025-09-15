/**
 * @file program/Window.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "program/Context.hpp"
#include "sol/types.hpp"
#include "system/Log.hpp"
#include "util/Definitions.hpp"

#include <memory>
#include <tuple>
#include <variant>

union SDL_Event;
struct SDL_KeyboardEvent;
struct SDL_Window;

namespace tudov
{
	class Engine;
	class ScriptEngine;
	class Renderer;
	struct AppEvent;
	struct EventHandleKey;

	struct IWindow : public IContextProvider
	{
		virtual void InitializeWindow(std::int32_t width, std::int32_t height, std::string_view title) noexcept = 0;

		virtual void DeinitializeWindow() noexcept = 0;

		virtual WindowID GetWindowID() const noexcept = 0;

		virtual std::int32_t GetWidth() const noexcept = 0;

		virtual std::int32_t GetHeight() const noexcept = 0;

		virtual std::float_t GetDisplayScale() const noexcept = 0;

		virtual std::float_t GetGUIScale() const noexcept = 0;

		virtual std::tuple<std::int32_t, std::int32_t> GetSize() const noexcept = 0;

		virtual bool IsMinimized() const noexcept = 0;

		virtual void Close() noexcept = 0;

		virtual bool ShouldClose() noexcept = 0;

		virtual void HandleEvents() noexcept = 0;

		virtual bool HandleEvent(AppEvent &appEvent) noexcept = 0;

		virtual void Render() noexcept = 0;
	};

	/**
	 * Abstract Class
	 */
	class Window : public IWindow
	{
		friend LuaBindings;

	  protected:
		// static ILuaBindings::TInstallation windowLuaBindingsInstallation;

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

	  public:
		Context &GetContext() noexcept override;
		WindowID GetWindowID() const noexcept override;
		std::int32_t GetWidth() const noexcept override;
		std::int32_t GetHeight() const noexcept override;
		std::tuple<std::int32_t, std::int32_t> GetSize() const noexcept override;
		std::float_t GetDisplayScale() const noexcept override;
		std::float_t GetGUIScale() const noexcept override;
		bool IsMinimized() const noexcept override;
		void Close() noexcept override;
		bool ShouldClose() noexcept override;
		void HandleEvents() noexcept override;
		bool HandleEvent(AppEvent &appEvent) noexcept override;
		void Render() noexcept override;

		SDL_Window *GetSDLWindowHandle() noexcept;

		virtual EventHandleKey GetKey() const noexcept;

	  protected:
		bool RenderPreImpl() noexcept;

	  private:
		bool HandleEventKey(SDL_KeyboardEvent &e) noexcept;

		std::variant<sol::nil_t, std::double_t, sol::string_view> LuaGetKey() const noexcept;
	};
} // namespace tudov
