#pragma once

#include "program/Window.hpp"

#include <string_view>

namespace tudov
{
	struct IDebugElement
	{
		virtual std::string_view GetName() noexcept = 0;
		virtual void UpdateAndRender(const std::shared_ptr<IWindow> &window) noexcept = 0;
	};
} // namespace tudov
