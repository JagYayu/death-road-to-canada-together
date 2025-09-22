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

#include "system/Mouse.hpp"

#include "program/Window.hpp"
#include "program/WindowManager.hpp"
#include "system/MouseButton.hpp"
#include "util/EnumFlag.hpp"

#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_video.h"

#include <memory>
#include <tuple>

using namespace tudov;

Mouse::Mouse(MouseID mouseID) noexcept
    : _mouseID(mouseID)
{
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

std::shared_ptr<Window> Mouse::LuaGetFocusedWindow() noexcept
{
	return std::dynamic_pointer_cast<Window>(GetFocusedWindow());
}

PrimaryMouse::PrimaryMouse(Context &context) noexcept
    : Mouse(0),
      _context(context)
{
}

Context &PrimaryMouse::GetContext() noexcept
{
	return _context;
}

std::shared_ptr<IWindow> PrimaryMouse::GetFocusedWindow() noexcept
{
	SDL_Window *sdlWindow = SDL_GetMouseFocus();
	return GetWindowManager().GetWindowByID(SDL_GetWindowID(sdlWindow));
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
