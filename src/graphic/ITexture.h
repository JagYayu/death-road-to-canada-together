#pragma once

#include "IRenderBackend.h"

namespace tudov
{
	struct IRenderer;

	struct ITexture : public IRenderBackend
	{
		IRenderer &renderer;

		explicit ITexture(IRenderer &renderer) noexcept;

		virtual void Test() {};
	};
} // namespace tudov
