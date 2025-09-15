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

#include "EngineComponent.hpp"
#include "system/Log.hpp"
#include "util/Definitions.hpp"

#include <memory>
#include <vector>

union SDL_Event;

namespace tudov
{
	class Context;
	struct AppEvent;
	struct IDebugManager;
	struct IWindow;

	class WindowManager : public IEngineComponent, private ILogProvider
	{
	  private:
		Context &_context;
		std::weak_ptr<IWindow> _primaryWindow;
		std::vector<std::shared_ptr<IWindow>> _subWindows;
		std::vector<std::shared_ptr<IWindow>> _windows;
		std::shared_ptr<IDebugManager> _debugManager;

	  public:
		explicit WindowManager(Context &context) noexcept;

		Context& GetContext() noexcept override;
		Log &GetLog() noexcept override;

		bool HandleEvent(AppEvent &appEvent) noexcept override;

		std::shared_ptr<IWindow> GetWindowByID(WindowID windowID) noexcept;
		std::shared_ptr<IWindow> GetPrimaryWindow() noexcept;
		std::shared_ptr<const IWindow> GetPrimaryWindow() const noexcept;
		std::vector<std::shared_ptr<IWindow>> &GetSubWindows() noexcept;
		const std::vector<std::shared_ptr<IWindow>> &GetSubWindows() const noexcept;

		std::shared_ptr<IDebugManager> GetDebugManager() noexcept;
		std::shared_ptr<const IDebugManager> GetDebugManager() const noexcept;

		void AddSubWindow(const std::shared_ptr<IWindow> &window);
		void RemoveSubWindow(const std::shared_ptr<IWindow> &window);

		bool IsEmpty() noexcept;
		void InitializePrimaryWindow() noexcept;
		void CloseWindows() noexcept;
		void HandleEvents() noexcept;
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
