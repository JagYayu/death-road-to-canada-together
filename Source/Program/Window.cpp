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

#include "audio/AudioPlayer.hpp"
#include "event/AppEvent.hpp"
#include "event/CoreEvents.hpp"
#include "event/CoreEventsData.hpp"
#include "event/EventHandleKey.hpp"
#include "event/EventManager.hpp"
#include "event/RuntimeEvent.hpp"
#include "graphic/Renderer.hpp"
#include "mod/ScriptEngine.hpp"
#include "program/Engine.hpp"
#include "program/WindowManager.hpp"
#include "system/ScanCode.hpp"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"
#include "sol/string_view.hpp"
#include "sol/types.hpp"

#include <cmath>
#include <memory>
#include <string_view>
#include <variant>

using namespace tudov;

Window::Window(Context &context, std::string_view logName) noexcept
    : _context(context),
      _renderer(std::make_shared<Renderer>(*this)),
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

IRenderer *Window::GetIRenderer() noexcept
{
	return _renderer.get();
}

IAudioPlayer *Window::GetIAudioPlayer() noexcept
{
	return nullptr; // _audioPlayer.get();
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
	return nullptr;
}

void Window::HandleEvents() noexcept
{
}

bool Window::HandleEvent(AppEvent &appEvent) noexcept
{
	SDL_Event &sdlEvent = *appEvent.sdlEvent;
	IEventManager &eventManager = GetEventManager();

	if (sdlEvent.type >= SDL_EVENT_WINDOW_FIRST && sdlEvent.type <= SDL_EVENT_WINDOW_LAST)
	{
		if (SDL_GetWindowID(_sdlWindow) != sdlEvent.window.windowID)
		{
			return false;
		}

		if (sdlEvent.type == SDL_EVENT_WINDOW_RESIZED)
		{
			SDL_WindowEvent &window = sdlEvent.window;
			SDL_WindowID windowID = SDL_GetWindowID(_sdlWindow);
			if (windowID != window.windowID)
			{
				return false;
			}

			RuntimeEvent &runtimeEvent = GetEventManager().GetCoreEvents().MouseMotion();

			EventWindowResizeData data{
			    // window.
			};

			runtimeEvent.Invoke(&data, EventHandleKey(windowID), EEventInvocation::None);
		}
	}
	else if (sdlEvent.type == SDL_EVENT_KEY_DOWN || sdlEvent.type == SDL_EVENT_KEY_UP)
	{
		// SDL_KeyboardEvent &key = sdlEvent.key;
		// SDL_WindowID windowID = SDL_GetWindowID(_sdlWindow);
		// if (windowID != key.windowID)
		// {
		// 	return false;
		// }

		// RuntimeEvent *runtimeEvent;
		// if (!key.down)
		// {
		// 	runtimeEvent = &GetEventManager().GetCoreEvents().KeyUp();
		// }
		// else if (key.repeat)
		// {
		// 	runtimeEvent = &GetEventManager().GetCoreEvents().KeyRepeat();
		// }
		// else
		// {
		// 	runtimeEvent = &GetEventManager().GetCoreEvents().KeyboardPress();
		// }

		// EventKeyData data{
		//     .window = this,
		//     .windowID = windowID,
		//     .keyboard = sol::nil,
		//     .keyboardID = 0,
		//     .scanCode = static_cast<EScanCode>(key.scancode),
		//     .keyCode = static_cast<EKeyCode>(key.key),
		//     .modifier = static_cast<EKeyModifier>(key.mod),
		// };

		// runtimeEvent->Invoke(&data, windowID, EEventInvocation::None);
	}
	else if (sdlEvent.type == SDL_EVENT_MOUSE_MOTION)
	{
		SDL_MouseMotionEvent &motion = sdlEvent.motion;
		SDL_WindowID windowID = SDL_GetWindowID(_sdlWindow);
		if (windowID != motion.windowID)
		{
			return false;
		}

		RuntimeEvent &runtimeEvent = GetEventManager().GetCoreEvents().MouseMotion();

		EventMouseMotionData data{
		    .mouseID = static_cast<std::int32_t>(motion.which),
		    .x = motion.x,
		    .y = motion.y,
		    .relativeX = motion.xrel,
		    .relativeY = motion.yrel,
		};

		runtimeEvent.Invoke(&data, EventHandleKey(windowID), EEventInvocation::None);
	}
	else
	{
		return false;
	}

	return true;
}

bool Window::HandleEventKey(SDL_KeyboardEvent &e) noexcept
{
	return true;
}

void Window::Update() noexcept
{
	// _audioPlayer->Update();
}

void Window::Render() noexcept
{
	_renderer->Begin();
	if (RenderPreImpl())
	{
		_renderer->End();
	}
}

bool Window::RenderPreImpl() noexcept
{
	if (IsMinimized())
	{
		return false;
	}

	auto &&args = GetScriptEngine().CreateTable(0, 3);
	args["isMain"] = GetContext().GetWindowManager().GetPrimaryWindow().get() == this;
	args["window"] = this;
	args["key"] = LuaGetKey();
	GetEventManager().GetCoreEvents().TickRender().Invoke(args, GetKey(), EEventInvocation::None);

	return true;
}

SDL_Window *Window::GetSDLWindowHandle() noexcept
{
	return _sdlWindow;
}

WindowID Window::GetWindowID() const noexcept
{
	return SDL_GetWindowID(_sdlWindow);
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

std::variant<sol::nil_t, std::double_t, sol::string_view> Window::LuaGetKey() const noexcept
{
	auto key = GetKey();
	if (std::holds_alternative<std::double_t>(key.value))
	{
		return std::get<std::double_t>(key.value);
	}
	else if (std::holds_alternative<std::string_view>(key.value))
	{
		return std::get<std::string_view>(key.value);
	}
	else
	{
		return sol::nil;
	}
}
