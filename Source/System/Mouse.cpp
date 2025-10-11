/**
 * @file program/Mouse.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "System/Mouse.hpp"

#include "Event/AppEvent.hpp"
#include "Event/CoreEvents.hpp"
#include "Event/CoreEventsData.hpp"
#include "Event/EventManager.hpp"
#include "Event/RuntimeEvent.hpp"
#include "Mod/ScriptEngine.hpp"
#include "Program/Window.hpp"
#include "Program/WindowManager.hpp"
#include "System/Keyboard.hpp"
#include "System/LogMicros.hpp"
#include "System/MouseButton.hpp"
#include "Util/EnumFlag.hpp"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_video.h"

#include <memory>
#include <tuple>

using namespace tudov;

Mouse::Mouse(Context &context, MouseID mouseID) noexcept
    : _context(context),
      _mouseID(mouseID)
{
}

Context &Mouse::GetContext() noexcept
{
	return _context;
}

Log &Mouse::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(Mouse));
}

MouseID Mouse::GetMouseID() const noexcept
{
	return _mouseID;
}

std::string_view Mouse::GetMouseName() const noexcept
{
	return SDL_GetMouseNameForID(GetMouseID());
}

std::shared_ptr<IWindow> Mouse::GetFocusedWindow() noexcept
{
	return nullptr;
}

std::tuple<std::float_t, std::float_t> Mouse::GetMousePosition() const noexcept
{
	std::float_t x, y;
	SDL_GetMouseState(&x, &y);
	return std::make_tuple(x, y);
}

bool Mouse::IsMouseButtonDown(EMouseButton button) const noexcept
{
	return false;
}

bool Mouse::IsMouseButtonsDown(EMouseButtonFlag buttonFlags) const noexcept
{
	return false;
}

std::tuple<std::float_t, std::float_t, EMouseButtonFlag> Mouse::GetMouseState() const noexcept
{
	return {};
}

bool Mouse::HandleEvent(AppEvent &appEvent) noexcept
{
	SDL_Event &sdlEvent = *appEvent.sdlEvent;

	switch (sdlEvent.type)
	{
	case SDL_EVENT_MOUSE_MOTION:
	{
		SDL_MouseMotionEvent &motion = sdlEvent.motion;
		if (motion.which != _mouseID)
		{
			return false;
		}

		SDL_WindowID windowID = motion.windowID;

		RuntimeEvent &runtimeEvent = GetEventManager().GetCoreEvents().MouseMotion();

		EventMouseMotionData data{
		    .window = std::dynamic_pointer_cast<Window>(GetWindowManager().GetIWindowByID(windowID)),
		    .windowID = windowID,
		    .mouse = _mouseID != 0 ? shared_from_this() : nullptr,
		    .mouseID = _mouseID,
		    .x = motion.x,
		    .y = motion.y,
		    .relativeX = motion.xrel,
		    .relativeY = motion.yrel,
		};

		runtimeEvent.Invoke(&data, EventHandleKey(windowID));

		break;
	}
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		break;
	case SDL_EVENT_MOUSE_BUTTON_UP:
		break;
	case SDL_EVENT_MOUSE_WHEEL:
		break;
	case SDL_EVENT_MOUSE_ADDED:
		break;
	case SDL_EVENT_MOUSE_REMOVED:
		break;
	default:
		return false;
	}

	TE_TRACE("Event handled by mouse: {}", _mouseID);

	return true;
}

std::shared_ptr<Window> Mouse::LuaGetFocusedWindow() noexcept
{
	return std::dynamic_pointer_cast<Window>(GetFocusedWindow());
}

PrimaryMouse::PrimaryMouse(Context &context) noexcept
    : Mouse(context, 0)
{
}

Log &PrimaryMouse::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(PrimaryMouse));
}

std::shared_ptr<IWindow> PrimaryMouse::GetFocusedWindow() noexcept
{
	SDL_Window *sdlWindow = SDL_GetMouseFocus();
	return GetWindowManager().GetIWindowByID(SDL_GetWindowID(sdlWindow));
}

std::tuple<std::float_t, std::float_t> PrimaryMouse::GetMousePosition() const noexcept
{
	std::float_t x, y;
	SDL_GetMouseState(&x, &y);
	return std::make_tuple(x, y);
}

bool PrimaryMouse::IsMouseButtonDown(EMouseButton button) const noexcept
{
	return (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_LMASK) != 0;
}

bool PrimaryMouse::IsMouseButtonsDown(EMouseButtonFlag buttonFlags) const noexcept
{
	return EnumFlag::HasAll(static_cast<EMouseButtonFlag>(SDL_GetMouseState(nullptr, nullptr)), buttonFlags);
}

std::tuple<std::float_t, std::float_t, EMouseButtonFlag> PrimaryMouse::GetMouseState() const noexcept
{
	std::float_t x, y;
	auto flags = SDL_GetMouseState(&x, &y);
	return std::make_tuple(x, y, static_cast<EMouseButtonFlag>(flags));
}
