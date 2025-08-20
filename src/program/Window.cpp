/**
 * @file program/Window.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "program/Window.hpp"

#include "event/EventManager.hpp"
#include "graphic/Renderer.hpp"
#include "mod/ScriptEngine.hpp"
#include "program/Engine.hpp"
#include "program/WindowManager.hpp"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"

#include <cmath>
#include <memory>

using namespace tudov;

Window::Window(Context &context, std::string_view logName) noexcept
    : _context(context),
      renderer(std::make_shared<Renderer>(*this)),
      _log(Log::Get(logName)),
      _sdlWindow(nullptr),
      _shouldClose(false)
{
}

Window::~Window() noexcept
{
}

Context &Window::GetContext() noexcept
{
	return _context;
}

bool Window::IsMinimized() const noexcept
{
	return (SDL_GetWindowFlags(_sdlWindow) & SDL_WINDOW_MINIMIZED) != 0;
}

void Window::Close() noexcept
{
	_shouldClose = true;
}

bool Window::ShouldClose() noexcept
{
	return _shouldClose;
}

EventHandleKey Window::GetKey() const noexcept
{
	return {};
}

void Window::HandleEvents() noexcept
{
}

bool Window::HandleEvent(SDL_Event &event) noexcept
{
	IEventManager &eventManager = GetEventManager();

	if (event.type >= SDL_EVENT_WINDOW_FIRST && event.type <= SDL_EVENT_WINDOW_LAST)
	{
		if (SDL_GetWindowID(_sdlWindow) != event.window.windowID)
		{
			return false;
		}
	}
	else if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
	{
		SDL_KeyboardEvent &key = event.key;
		SDL_WindowID windowID = SDL_GetWindowID(_sdlWindow);
		if (windowID != key.windowID)
		{
			return false;
		}

		RuntimeEvent *runtimeEvent;
		if (event.type == SDL_EVENT_KEY_UP)
		{
			runtimeEvent = &GetEventManager().GetCoreEvents().KeyUp();
		}
		else if (key.repeat)
		{
			runtimeEvent = &GetEventManager().GetCoreEvents().KeyRepeat();
		}
		else
		{
			runtimeEvent = &GetEventManager().GetCoreEvents().KeyDown();
		}

		sol::table e = GetScriptEngine().CreateTable(0, 3);
		e["window"] = this;
		e["windowID"] = windowID;
		e[""] = key.down;

		runtimeEvent->Invoke(e, windowID);
	}
	else
	{
		return false;
	}

	return true;
}

bool Window::HandleEventKey(SDL_KeyboardEvent &e) noexcept
{
	// if (SDL_GetWindowID(_sdlWindow) == e.windowID)
	// {
	// 	if (e.type != SDL_EVENT_KEY_DOWN)
	// 	{
	// 	}
	// }
	return 1;
}

void Window::Render() noexcept
{
	renderer->Begin();
	if (RenderPreImpl())
	{
		renderer->Render();
		renderer->End();
	}
}

bool Window::RenderPreImpl() noexcept
{
	if (IsMinimized())
	{
		return false;
	}

	auto &&args = GetScriptEngine().CreateTable(0, 3);
	auto &&key = GetKey();
	args["isMain"] = GetContext().GetWindowManager().GetMainWindow().get() == this;
	args["key"] = &key;
	args["window"] = this;
	GetEventManager().GetCoreEvents().TickRender().Invoke(args, key);

	return true;
}

SDL_Window *Window::GetSDLWindowHandle() noexcept
{
	return _sdlWindow;
}

std::int32_t Window::GetWidth() const noexcept
{
	std::int32_t w;
	SDL_GetWindowSize(_sdlWindow, &w, nullptr);
	return w;
}

std::int32_t Window::GetHeight() const noexcept
{
	std::int32_t h;
	SDL_GetWindowSize(_sdlWindow, nullptr, &h);
	return h;
}

std::tuple<std::int32_t, std::int32_t> Window::GetSize() const noexcept
{
	std::int32_t w, h;
	SDL_GetWindowSize(_sdlWindow, &w, &h);
	return {w, h};
}

std::float_t Window::GetDisplayScale() const noexcept
{
	return SDL_GetDisplayContentScale(SDL_GetDisplayForWindow(_sdlWindow));
}

std::float_t Window::GetGUIScale() const noexcept
{
	std::int32_t w, h;
	SDL_GetWindowSize(_sdlWindow, &w, &h);
	return std::min(w, h) / 720.0f;
}
