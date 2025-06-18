#pragma once

#include "ERenderBackend.h"

namespace tudov
{
	struct IRenderBackend
	{
		virtual ERenderBackend GetRenderBackend() const noexcept = 0;
	};
} // namespace tudov
