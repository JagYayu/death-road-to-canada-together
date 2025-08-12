#include "debug/DebugUtils.hpp"

#include "program/Window.hpp"

using namespace tudov;

std::float_t DebugUtils::GetWindowGUIScale(IWindow &window) noexcept
{
	return window.GetGUIScale();
}