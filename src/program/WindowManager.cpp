#include "program/WindowManager.hpp"

#include "SDL3/SDL_events.h"
#include "data/Config.hpp"
#include "debug/DebugManager.hpp"
#include "program/MainWindow.hpp"
#include "util/Log.hpp"
#include "util/MicrosImpl.hpp"

#include <algorithm>
#include <stdexcept>

using namespace tudov;

WindowManager::WindowManager(Context &context) noexcept
    : _context(context)
{
}

TE_GEN_GETTER_SMART_PTR(std::shared_ptr, IWindow, WindowManager::GetMainWindow, _mainWindow, noexcept);

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
	auto &&it = std::find(_subWindows.begin(), _subWindows.end(), window);
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

void WindowManager::InitializeMainWindow() noexcept
{
	if (!_mainWindow.expired())
	{
		Log::Get("WindowManager")->Error("Engine main window has already been initialized!");
	}

	auto &&mainWindow = std::make_shared<MainWindow>(_context);
	auto &config = _context.GetConfig();
	mainWindow->InitializeWindow(config.GetWindowWidth(), config.GetWindowHeight(), config.GetWindowTitle());
	_windows.emplace_back(mainWindow);
	_mainWindow = mainWindow;

	_debugManager = std::make_shared<DebugManager>();
	mainWindow->SetDebugManager(_debugManager);
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
	// for (auto &&window : _windows)
	// {
	// 	window->HandleEvents();
	// }
	//
	// _windows.erase(std::remove_if(_windows.begin(), _windows.end(), IsWindowShouldClose), _windows.end());
}

void WindowManager::HandleEvents(SDL_Event &sdlEvent) noexcept
{
	for (auto &&window : _windows)
	{
		window->HandleEvent(sdlEvent);
	}

	_windows.erase(std::remove_if(_windows.begin(), _windows.end(), IsWindowShouldClose), _windows.end());
}

void WindowManager::Render() noexcept
{
	for (auto &&window : _windows)
	{
		window->Render();
	}
}
