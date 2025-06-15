#pragma once

#include "util/Defs.h"

namespace tudov
{
	struct IDebugElement
	{
		virtual std::string_view GetName() noexcept = 0;
		virtual void UpdateAndRender() noexcept = 0;
	};
} // namespace tudov
