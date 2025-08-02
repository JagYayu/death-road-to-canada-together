#include "test/TestGPURendering.hpp"

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_timer.h"

using namespace tudov;

void TestGPURendering::Init(void *appstate) noexcept
{
}

int TestGPURendering::Iterate(void *appstate) noexcept
{
	SDL_Delay(1);

	return SDL_APP_CONTINUE;
}

int TestGPURendering::Event(void *appstate, SDL_Event *event) noexcept
{
	return SDL_APP_CONTINUE;
}

void TestGPURendering::Quit(void *appstate) noexcept
{
}
