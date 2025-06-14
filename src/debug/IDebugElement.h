#pragma once

#include "util/Defs.h"

namespace tudov
{
	struct IDebugElement
	{
		virtual StringView GetName() noexcept = 0;
		virtual void UpdateAndRender() noexcept = 0;
	};
} // namespace tudov
