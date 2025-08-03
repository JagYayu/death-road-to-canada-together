#pragma once

#define TE_TEST_GPU_RENDERING

#include "util/Micros.hpp"

#include <memory>

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
