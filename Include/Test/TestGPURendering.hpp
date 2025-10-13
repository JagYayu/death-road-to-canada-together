/**
 * @file resource/TextGPURendering.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Util/Micros.hpp"

union SDL_Event;

namespace tudov
{
	class Log;

	class TestGPURendering
	{
	  public:
		TE_STATIC_CLASS(TestGPURendering);

		static void Init(void *appstate) noexcept;
		static int Iterate(void *appstate) noexcept;
		static int Event(void *appstate, SDL_Event *event) noexcept;
		static void Quit(void *appstate) noexcept;
	};
} // namespace tudov
