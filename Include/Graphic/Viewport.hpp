/**
 * @file graphic/Viewport.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

struct SDL_GPUViewport;

namespace tudov
{
	class Viewport
	{
	  private:
		SDL_GPUViewport *_viewport;

	  public:
		explicit Viewport() noexcept;
	};
} // namespace tudov
