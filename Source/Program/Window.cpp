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

#include "Program/Window.hpp"

#include "Audio/AudioPlayer.hpp"
#include "Event/AppEvent.hpp"
#include "Event/CoreEvents.hpp"
#include "Event/CoreEventsData.hpp"
#include "Event/EventHandleKey.hpp"
#include "Event/EventManager.hpp"
#include "Event/RuntimeEvent.hpp"
#include "Graphic/Renderer.hpp"
#include "Mod/ScriptEngine.hpp"
#include "Program/Engine.hpp"
#include "Program/WindowManager.hpp"
#include "System/LogMicros.hpp"
#include "System/ScanCode.hpp"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"
#include "sol/string_view.hpp"
#include "sol/types.hpp"

#include <cmath>
#include <memory>
#include <string>
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

Log &Window::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(Window));
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
	else
	{
		return false;
	}

	TE_TRACE("Event handled by window {}", reinterpret_cast<std::uint64_t>(_sdlWindow));

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
