#pragma once

#include "Window.hpp"

namespace tudov
{
	class Engine;
	struct IDebugManager;
	class RenderTarget;

	class MainWindow : public Window
	{
	  private:
		std::weak_ptr<IDebugManager> _debugManager;
		RenderTarget *_debugRenderTarget;

	  public:
		explicit MainWindow(Context &context) noexcept;
		~MainWindow() noexcept override;

	  private:
		void RenderLoadingGUI(Engine &engine) noexcept;

	  public:
		virtual void Initialize(std::int32_t width, std::int32_t height, std::string_view title) noexcept override;
		virtual EventHandleKey GetKey() const noexcept override;
		virtual bool HandleEvent(SDL_Event &event) noexcept override;
		virtual void Render() noexcept override;

		void SetDebugManager(const std::shared_ptr<IDebugManager> &debugManager) noexcept;
	};
} // namespace tudov
