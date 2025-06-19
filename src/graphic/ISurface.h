#pragma once

#include "IRenderBackend.h"
#include "util/Color.hpp"

#include "glm/ext/vector_float2.hpp"

namespace tudov
{
	struct IRenderer;

	struct ISurface
	{
		IRenderer &renderer;

		explicit ISurface(IRenderer &renderer) noexcept;

		virtual std::int32_t GetWidth() const noexcept = 0;
		virtual std::int32_t GetHeight() const noexcept = 0;
		virtual Color GetPixel(std::int32_t index) const noexcept = 0;
		virtual Color GetPixel(std::int32_t x, std::int32_t y) const noexcept = 0;
		glm::vec2 GetSize() const noexcept;
	};
} // namespace tudov
