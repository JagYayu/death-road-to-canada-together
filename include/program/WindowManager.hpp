/**
 * @file program/WindowManager.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "debug/DebugManager.hpp"
#include "util/Log.hpp"

#include <memory>
#include <vector>

union SDL_Event;
using SDL_WindowID = std::uint32_t;

namespace tudov
{
	class Context;
	struct IDebugManager;
	struct IWindow;

	class WindowManager : private ILogProvider
	{
	  private:
		Context &_context;
		std::weak_ptr<IWindow> _mainWindow;
		std::vector<std::shared_ptr<IWindow>> _subWindows;
		std::vector<std::shared_ptr<IWindow>> _windows;
		std::shared_ptr<IDebugManager> _debugManager;

	  public:
		explicit WindowManager(Context &context) noexcept;

		Log &GetLog() noexcept override;

		std::shared_ptr<IWindow> GetWindowByID(SDL_WindowID windowID) noexcept;

		std::shared_ptr<IWindow> GetMainWindow() noexcept;
		std::shared_ptr<const IWindow> GetMainWindow() const noexcept;
		std::vector<std::shared_ptr<IWindow>> &GetSubWindows() noexcept;
		const std::vector<std::shared_ptr<IWindow>> &GetSubWindows() const noexcept;

		std::shared_ptr<IDebugManager> GetDebugManager() noexcept;
		std::shared_ptr<const IDebugManager> GetDebugManager() const noexcept;

		void AddSubWindow(const std::shared_ptr<IWindow> &window);
		void RemoveSubWindow(const std::shared_ptr<IWindow> &window);

		bool IsEmpty() noexcept;
		void InitializeMainWindow() noexcept;
		void CloseWindows() noexcept;
		void HandleEvents() noexcept;
		bool HandleEvent(SDL_Event &sdlEvent) noexcept;
		void Render() noexcept;

		inline std::vector<std::shared_ptr<IWindow>>::iterator begin() noexcept
		{
			return _windows.begin();
		}

		inline std::vector<std::shared_ptr<IWindow>>::iterator end() noexcept
		{
			return _windows.end();
		}
	};
} // namespace tudov
