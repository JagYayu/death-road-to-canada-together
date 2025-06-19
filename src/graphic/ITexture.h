#pragma once

#include "IRenderBackend.h"
#include "ISurface.h"

#include <memory>

namespace tudov
{
	struct IRenderer;

	struct ITexture : public IRenderBackend
	{
		IRenderer &renderer;
		std::shared_ptr<ISurface> surface;

		explicit ITexture(IRenderer &renderer) noexcept;
	};
} // namespace tudov
