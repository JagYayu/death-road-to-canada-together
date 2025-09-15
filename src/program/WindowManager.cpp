/**
 * @file program/WindowManager.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "program/WindowManager.hpp"

#include "data/Config.hpp"
#include "debug/DebugManager.hpp"
#include "program/PrimaryWindow.hpp"
#include "system/Log.hpp"
#include "util/MicrosImpl.hpp"

#include <algorithm>
#include <memory>
#include <stdexcept>

using namespace tudov;

WindowManager::WindowManager(Context &context) noexcept
    : _context(context)
{
}

Context &WindowManager::GetContext() noexcept
{
	return _context;
}

Log &WindowManager::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(WindowManager));
}

std::shared_ptr<IWindow> WindowManager::GetWindowByID(WindowID windowID) noexcept
{
	for (std::shared_ptr<IWindow> &window : _windows)
	{
		if (window->GetWindowID() == windowID)
		{
			return window;
		}
	}
	return nullptr;
}

TE_GEN_GETTER_SMART_PTR(std::shared_ptr, IWindow, WindowManager::GetPrimaryWindow, _primaryWindow, noexcept);

std::vector<std::shared_ptr<IWindow>> &WindowManager::GetSubWindows() noexcept
{
	return _subWindows;
}

const std::vector<std::shared_ptr<IWindow>> &WindowManager::GetSubWindows() const noexcept
{
	return _subWindows;
}

std::shared_ptr<IDebugManager> WindowManager::GetDebugManager() noexcept
{
	return _debugManager;
}

std::shared_ptr<const IDebugManager> WindowManager::GetDebugManager() const noexcept
{
	return _debugManager;
}

void WindowManager::AddSubWindow(const std::shared_ptr<IWindow> &window)
{
	for (auto &window : _subWindows)
	{
		if (window.get() == window.get()) [[unlikely]]
		{
			throw std::runtime_error("Duplicate adding sub window");
		}
	}

	_subWindows.emplace_back(window);
	_windows.emplace_back(window);
}

void WindowManager::RemoveSubWindow(const std::shared_ptr<IWindow> &window)
{
	auto it = std::find(_subWindows.begin(), _subWindows.end(), window);
	if (it == _subWindows.end())
	{
		throw std::runtime_error("Failure remove sub window");
	}

	_subWindows.erase(it);
	it = std::find(_windows.begin(), _windows.end(), window);
	_windows.erase(it);
}

bool WindowManager::IsEmpty() noexcept
{
	return _windows.empty();
}

void WindowManager::InitializePrimaryWindow() noexcept
{
	if (!_primaryWindow.expired())
	{
		Log::Get("WindowManager")->Error("Engine main window has already been initialized!");
	}

	const auto &primaryWindow = std::make_shared<PrimaryWindow>(_context);
	Config &config = Tudov::GetConfig();
	primaryWindow->InitializeWindow(config.GetWindowWidth(), config.GetWindowHeight(), config.GetWindowTitle());
	_windows.emplace_back(primaryWindow);
	_primaryWindow = primaryWindow;

	_debugManager = std::make_shared<DebugManager>();
	primaryWindow->SetDebugManager(_debugManager);
}

void WindowManager::CloseWindows() noexcept
{
	for (auto &&window : _windows)
	{
		window->Close();
	}

	_windows.clear();
	_subWindows.clear();
}

bool IsWindowShouldClose(const std::shared_ptr<IWindow> &window) noexcept
{
	return window->ShouldClose();
}

void WindowManager::HandleEvents() noexcept
{
}

bool WindowManager::HandleEvent(AppEvent &appEvent) noexcept
{
	bool handled = false;
	for (const std::shared_ptr<IWindow> &window : _windows)
	{
		handled = window->HandleEvent(appEvent) || handled;
	}

	_windows.erase(std::remove_if(_windows.begin(), _windows.end(), IsWindowShouldClose), _windows.end());

	return handled;
}

void WindowManager::Render() noexcept
{
	for (auto &&window : _windows)
	{
		window->Render();
	}
}
