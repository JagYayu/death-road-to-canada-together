/**
 * @file debug/DebugUtils.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "debug/DebugUtils.hpp"

#include "program/Window.hpp"

using namespace tudov;

std::float_t DebugUtils::GetWindowGUIScale(IWindow &window) noexcept
{
	return window.GetGUIScale();
}
