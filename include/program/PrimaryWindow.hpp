/**
 * @file program/PrimaryWindow.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Window.hpp"

struct ImFont;
union SDL_Event;

namespace tudov
{
	class Engine;
	struct IDebugManager;
	class RenderTarget;

	class PrimaryWindow : public Window
	{
	  protected:
		std::weak_ptr<IDebugManager> _debugManager;
		std::shared_ptr<RenderTarget> _renderTarget;
		std::string _guiFontMemory;
		ImFont *_guiFontSmall;
		ImFont *_guiFontMedium;
		ImFont *_guiFontLarge;
		std::float_t _guiScale;
		bool _showDebugElements;

	  public:
		explicit PrimaryWindow(Context &context) noexcept;
		~PrimaryWindow() noexcept override;

	  protected:
		void UpdateGuiStyle() noexcept;
		void RenderLoadingGUI(Engine &engine) noexcept;

	  public:
		void InitializeWindow(std::int32_t width, std::int32_t height, std::string_view title) noexcept override;
		void DeinitializeWindow() noexcept override;

		EventHandleKey GetKey() const noexcept override;
		bool HandleEvent(AppEvent &appEvent) noexcept override;
		void Render() noexcept override;

		ImFont *GetGUIFontSmall() noexcept;
		ImFont *GetGUIFontMedium() noexcept;
		ImFont *GetGUIFontLarge() noexcept;

		void SetDebugManager(const std::shared_ptr<IDebugManager> &debugManager) noexcept;

		bool GetShowDebugElements() noexcept;
		void SetShowDebugElements(bool value) noexcept;
		bool GetShowLoadingFrame() noexcept;
		void SetShowLoadingFrame(bool value) noexcept;
	};
} // namespace tudov
