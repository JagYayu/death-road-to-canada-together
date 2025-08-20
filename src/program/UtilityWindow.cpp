/**
 * @file program/UtilityWindow.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "program/UtilityWindow.hpp"

#include "event/EventHandleKey.hpp"
#include "graphic/Renderer.hpp"

#include "SDL3/SDL_video.h"

#include <memory>
#include <string_view>

using namespace tudov;

EventHandleKey MakeUtilityWindowKey(std::string_view keyName) noexcept
{
	if (keyName.empty())
	{
		return "Utility";
	}
	else
	{
		return (std::string("Utility_") + keyName.data()).data();
	}
}

UtilityWindow::UtilityWindow(Context &context, std::string_view logName, std::string_view keyName) noexcept
    : Window(context, logName),
      _key(std::make_unique<EventHandleKey>(MakeUtilityWindowKey(keyName)))
{
}

UtilityWindow::~UtilityWindow() noexcept
{
	if (_sdlWindow != nullptr)
	{
		DeinitializeWindow();
	}
}

EventHandleKey UtilityWindow::GetKey() const noexcept
{
	return "Utility_";
}

void UtilityWindow::InitializeWindow(std::int32_t width, std::int32_t height, std::string_view title) noexcept
{
	_sdlWindow = SDL_CreateWindow(title.data(), width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_UTILITY);
	if (!_sdlWindow)
	{
		_log->Fatal("Failed to initialize SDL3 Window");
	}

	renderer->InitializeRenderer();
}

void UtilityWindow::DeinitializeWindow() noexcept
{
	if (_sdlWindow != nullptr)
	{
		SDL_DestroyWindow(_sdlWindow);
		_sdlWindow = nullptr;
	}
}
