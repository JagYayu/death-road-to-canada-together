#pragma once

#include "Camera2D.h"

#include <vector>

namespace tudov
{
	class Engine;

	class RenderTarget
	{
	  private:
	  public:
		explicit RenderTarget(Window &window) noexcept;

		void Begin() noexcept;
		void End() noexcept;
	};
} // namespace tudov
