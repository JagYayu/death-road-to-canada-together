#include "test/TestGPURendering.hpp"

#include "SDL3/SDL.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"

using namespace tudov;

SDL_Window *mainWindow;
SDL_Window *subWindow;
SDL_GPUDevice *gpuDevice;

void TestGPURendering::Init(void *appstate) noexcept
{
	mainWindow = SDL_CreateWindow("Main Window", 1920, 1080, {});
	subWindow = SDL_CreateWindow("Sub Window", 1920, 1080, {});

	SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
}

int TestGPURendering::Iterate(void *appstate) noexcept
{
	SDL_Delay(1);
	return SDL_APP_CONTINUE;
}

int TestGPURendering::Event(void *appstate, SDL_Event *event) noexcept
{
	switch (event->type)
	{
	case SDL_EventType::SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;
	}

	return SDL_APP_CONTINUE;
}

void TestGPURendering::Quit(void *appstate) noexcept
{
}
