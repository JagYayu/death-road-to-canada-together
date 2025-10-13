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
#include "System/Log.hpp"
#include "Util/Definitions.hpp"

#include <memory>
#include <vector>

union SDL_Event;

namespace tudov
{
	class Context;
	struct AppEvent;
	struct IDebugManager;
	struct IWindow;

	struct IWindowManager : public IEngineComponent
	{
		virtual ~IWindowManager() noexcept = default;

		virtual std::shared_ptr<IWindow> GetIWindowByID(WindowID windowID) noexcept = 0;

		virtual std::shared_ptr<IWindow> GetPrimaryWindow() noexcept = 0;

		virtual std::shared_ptr<const IWindow> GetPrimaryIWindow() const noexcept = 0;

		virtual std::vector<std::shared_ptr<IWindow>> &GetSubIWindows() noexcept = 0;

		virtual const std::vector<std::shared_ptr<IWindow>> &GetSubIWindows() const noexcept = 0;

		virtual std::shared_ptr<IDebugManager> GetIDebugManager() noexcept = 0;

		virtual std::shared_ptr<const IDebugManager> GetIDebugManager() const noexcept = 0;

		virtual void AddSubIWindow(const std::shared_ptr<IWindow> &window) = 0;

		virtual void RemoveSubIWindow(const std::shared_ptr<IWindow> &window) = 0;

		virtual bool IsEmpty() noexcept = 0;

		virtual void InitializePrimaryWindow() noexcept = 0;

		virtual void CloseWindows() noexcept = 0;

		virtual void HandleEvents() noexcept = 0;

		virtual void Update() noexcept = 0;

		virtual void Render() noexcept = 0;
	};

	class WindowManager final : public IWindowManager, private ILogProvider
	{
	  private:
		Context &_context;
		std::shared_ptr<IWindow> _primaryWindow;
		std::vector<std::shared_ptr<IWindow>> _subWindows;
		std::vector<std::shared_ptr<IWindow>> _windows;
		std::shared_ptr<IDebugManager> _debugManager;

	  public:
		explicit WindowManager(Context &context) noexcept;
		explicit WindowManager(const WindowManager &) noexcept = delete;
		explicit WindowManager(WindowManager &&) noexcept = delete;
		WindowManager &operator=(const WindowManager &) noexcept = delete;
		WindowManager &operator=(WindowManager &&) noexcept = delete;
		~WindowManager() noexcept override = default;

		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		bool HandleEvent(AppEvent &appEvent) noexcept override;

		std::shared_ptr<IWindow> GetIWindowByID(WindowID windowID) noexcept override;
		std::shared_ptr<IWindow> GetPrimaryWindow() noexcept override;
		std::shared_ptr<const IWindow> GetPrimaryIWindow() const noexcept override;
		std::vector<std::shared_ptr<IWindow>> &GetSubIWindows() noexcept override;
		const std::vector<std::shared_ptr<IWindow>> &GetSubIWindows() const noexcept override;

		std::shared_ptr<IDebugManager> GetIDebugManager() noexcept override;
		std::shared_ptr<const IDebugManager> GetIDebugManager() const noexcept override;

		void AddSubIWindow(const std::shared_ptr<IWindow> &window) override;
		void RemoveSubIWindow(const std::shared_ptr<IWindow> &window) override;

		bool IsEmpty() noexcept override;
		void InitializePrimaryWindow() noexcept override;
		void CloseWindows() noexcept override;
		void HandleEvents() noexcept override;
		void Update() noexcept override;
		void Render() noexcept override;

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
