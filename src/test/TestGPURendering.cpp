#include "test/TestGPURendering.hpp"

#include "util/Log.hpp"
#include <memory>

#include "SDL3/SDL.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"

using namespace tudov;

static std::shared_ptr<Log> gLog = nullptr;

SDL_Window *gpuWindowMain;
SDL_Window *gpuWindowSub;
SDL_GPUDevice *gpuDevice;

void TestGPURendering::Init(void *appstate) noexcept
{
	gLog = Log::Get("TestGPURendering");

	gpuWindowMain = SDL_CreateWindow("Main Window", 1920, 1080, SDL_WINDOW_RESIZABLE);
	gpuWindowSub = SDL_CreateWindow("Sub Window", 960, 540, SDL_WINDOW_RESIZABLE);

	gpuDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_PRIVATE |
	                                    SDL_GPU_SHADERFORMAT_SPIRV |
	                                    SDL_GPU_SHADERFORMAT_DXBC |
	                                    SDL_GPU_SHADERFORMAT_DXIL |
	                                    SDL_GPU_SHADERFORMAT_MSL |
	                                    SDL_GPU_SHADERFORMAT_METALLIB,
	                                true, nullptr);
	if (!gpuDevice)
	{
		gLog->Fatal("Failed to create gpu device: {}", SDL_GetError());
	}

	if (!SDL_ClaimWindowForGPUDevice(gpuDevice, gpuWindowMain))
	{
		gLog->Fatal("Cannot claim window for gpu device: {}", SDL_GetError());
	}

	if (!SDL_ClaimWindowForGPUDevice(gpuDevice, gpuWindowSub))
	{
		gLog->Fatal("Cannot claim window for gpu device: {}", SDL_GetError());
	}
}

void ProcessWindow(SDL_Window *sdlWindow) noexcept
{
	// 1. require command buffer
	// 2. swapchain texture
	// -- loop render passes
	//    3. begin render pass
	//    4. draw stuff
	//    5. end render pass
	// -- loop render passes
	// 6. submit command buffer

	// 1
	SDL_GPUCommandBuffer *gpuCommandBuffer = SDL_AcquireGPUCommandBuffer(gpuDevice);
	if (gpuCommandBuffer == nullptr)
	{
		gLog->Fatal("Failed to acquire gpu command buffer: {}", SDL_GetError());
	}

	// 2
	SDL_GPUTexture *swapchainTexture;
	if (!SDL_WaitAndAcquireGPUSwapchainTexture(gpuCommandBuffer, gpuWindowMain, &swapchainTexture, nullptr, nullptr))
	{
		gLog->Fatal("Failed to acquire gpu swapchain texture: {}", SDL_GetError());
		swapchainTexture = nullptr;
	}

	// 3
	SDL_GPUColorTargetInfo colorTargetInfo{
	    .texture = swapchainTexture,
	    // .mip_level = 0,
	    // .layer_or_depth_plane = 0,
	    .clear_color = SDL_FColor(0, 0, 0, 1),
	    .load_op = SDL_GPU_LOADOP_CLEAR,
	    .store_op = SDL_GPU_STOREOP_STORE,
	    // .resolve_texture = nullptr,
	    // .resolve_mip_level = nullptr,
	    // .resolve_layer = nullptr,
	    // .cycle = nullptr,
	    // .cycle_resolve_texture = nullptr,
	    // .padding1 = nullptr,
	    // .padding2 = nullptr,
	};
	SDL_GPURenderPass *gpuRenderPass = SDL_BeginGPURenderPass(gpuCommandBuffer, &colorTargetInfo, 1, nullptr);
	if (gpuRenderPass == nullptr)
	{
		gLog->Fatal("Failed to begin gpu render pass");
	}

	// 4

	// 5
	SDL_EndGPURenderPass(gpuRenderPass);

	// 6
	if (!SDL_SubmitGPUCommandBuffer(gpuCommandBuffer))
	{
		gLog->Fatal("Failed to submit gpu command buffer");
	}
}

int TestGPURendering::Iterate(void *appstate) noexcept
{
	if (gpuWindowMain)
	{
		ProcessWindow(gpuWindowMain);
	}
	if (gpuWindowSub)
	{
		ProcessWindow(gpuWindowSub);
	}

	SDL_Delay(1);
	return SDL_APP_CONTINUE;
}

int TestGPURendering::Event(void *appstate, SDL_Event *event) noexcept
{
	gLog->Trace("sdl event: {}", event->type);

	switch (event->type)
	{
	case SDL_EventType::SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;
	case SDL_EventType::SDL_EVENT_WINDOW_CLOSE_REQUESTED:
	{
		if (SDL_GetWindowID(gpuWindowMain) == event->window.windowID)
		{
			SDL_DestroyWindow(gpuWindowMain);
			gpuWindowMain = nullptr;
		}
		if (SDL_GetWindowID(gpuWindowSub) == event->window.windowID)
		{
			SDL_DestroyWindow(gpuWindowSub);
			gpuWindowSub = nullptr;
		}
		break;
	}
	}

	return SDL_APP_CONTINUE;
}

void TestGPURendering::Quit(void *appstate) noexcept
{
	gLog = nullptr;
}
