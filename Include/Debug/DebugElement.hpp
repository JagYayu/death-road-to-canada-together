/**
 * @file debug/DebugElement.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Program/Window.hpp"

namespace tudov
{
	struct IDebugElement
	{
		virtual ~IDebugElement() noexcept = default;

		/**
		 * Get the debug element's name, this will display on top bar.
		 */
		virtual std::string_view GetName() noexcept = 0;

		/**
		 * Invoke when this debug window is opened by user.
		 */
		virtual void OnOpened(IWindow &window) noexcept {};

		/**
		 * Invoke when this debug window is opened by user.
		 */
		virtual void OnClosed(IWindow &window) noexcept {};

		/**
		 * Update and render the debug window.
		 */
		virtual void UpdateAndRender(IWindow &window) noexcept = 0;
	};
} // namespace tudov
