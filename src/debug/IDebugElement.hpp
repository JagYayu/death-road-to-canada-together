#pragma once

#include "program/Window.hpp"

namespace tudov
{
	struct IDebugElement
	{
		virtual ~IDebugElement() noexcept = default;

		virtual std::string_view GetName() noexcept = 0;
		virtual void UpdateAndRender(IWindow &window) noexcept = 0;
	};
} // namespace tudov
