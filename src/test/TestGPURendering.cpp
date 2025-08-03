#include "test/TestGPURendering.hpp"

#include "SDL3/SDL_iostream.h"
#include "util/Log.hpp"
#include <cstdint>
#include <memory>
#include <tuple>
#include <vector>

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"

using namespace tudov;

static std::shared_ptr<Log> gLog = nullptr;

inline SDL_Window *window = nullptr;
inline SDL_GPUDevice *device = nullptr;
inline SDL_GPUShader *shaderFrag = nullptr;
inline SDL_GPUShader *shaderVert = nullptr;

struct DrawWindowContentsArgs
{
	SDL_GPUCommandBuffer *commandBuffer;
	SDL_GPURenderPass *renderPass;
};

void RenderWindow(DrawWindowContentsArgs &args) noexcept;

std::tuple<std::vector<std::uint8_t>, std::size_t> ReadFile(std::string_view filePath) noexcept
{
	SDL_IOStream *io = SDL_IOFromFile(filePath.data(), "rb");
	if (!io)
	{
		gLog->Fatal("Failed to open file: %s", SDL_GetError());
	}

	Sint64 ioSize = SDL_GetIOSize(io);
	if (ioSize <= 0)
	{
		gLog->Fatal("Invalid file size: %s");
	}

	std::vector<std::uint8_t> file;
	file.resize(static_cast<size_t>(ioSize));
	size_t readSize = SDL_ReadIO(io, file.data(), static_cast<size_t>(ioSize));
	SDL_CloseIO(io);

	return {file, readSize};
}

void TestGPURendering::Init(void *appstate) noexcept
{
	gLog = Log::Get("TestGPURendering");

	window = SDL_CreateWindow("Main Window", 1920, 1080, SDL_WINDOW_RESIZABLE);

	device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_PRIVATE |
	                                 SDL_GPU_SHADERFORMAT_SPIRV |
	                                 SDL_GPU_SHADERFORMAT_DXBC |
	                                 SDL_GPU_SHADERFORMAT_DXIL |
	                                 SDL_GPU_SHADERFORMAT_MSL |
	                                 SDL_GPU_SHADERFORMAT_METALLIB,
	                             true, "vulkan");

	if (!SDL_ClaimWindowForGPUDevice(device, window))
	{
		gLog->Fatal("Cannot claim window for gpu device: {}", SDL_GetError());
	}

	{
		auto [file, fileSize] = ReadFile("dev/shader/TestShader.vert.spv");
		SDL_GPUShaderCreateInfo info{
		    .code_size = fileSize,
		    .code = file.data(),
		    .entrypoint = "main",
		    .format = SDL_GPU_SHADERFORMAT_SPIRV,
		    .stage = SDL_GPU_SHADERSTAGE_VERTEX,
		};
		shaderVert = SDL_CreateGPUShader(device, &info);
	}

	{
		auto [file, fileSize] = ReadFile("dev/shader/TestShader.frag.spv");
		SDL_GPUShaderCreateInfo info{
		    .code_size = fileSize,
		    .code = file.data(),
		    .entrypoint = "main",
		    .format = SDL_GPU_SHADERFORMAT_SPIRV,
		    .stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
		};
		shaderFrag = SDL_CreateGPUShader(device, &info);
	}
}

int TestGPURendering::Iterate(void *appstate) noexcept
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
	SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device);
	if (commandBuffer == nullptr)
	{
		gLog->Fatal("Failed to acquire gpu command buffer: {}", SDL_GetError());
	}

	// 2
	SDL_GPUTexture *swapchainTexture;
	if (!SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window, &swapchainTexture, nullptr, nullptr))
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
	SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);
	if (renderPass == nullptr)
	{
		gLog->Fatal("Failed to begin gpu render pass");
	}

	// 4
	DrawWindowContentsArgs args{
	    .commandBuffer = commandBuffer,
	    .renderPass = renderPass,
	};
	RenderWindow(args);

	// 5
	SDL_EndGPURenderPass(renderPass);

	// 6
	if (!SDL_SubmitGPUCommandBuffer(commandBuffer))
	{
		gLog->Fatal("Failed to submit gpu command buffer");
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
		// if (SDL_GetWindowID(mainWindow) == event->windowID)
		// {
		// 	SDL_DestroyWindow(mainWindow);
		// 	mainWindow = nullptr;
		// }
		// if (SDL_GetWindowID(gpuWindowSub) == event->windowID)
		// {
		// 	SDL_DestroyWindow(gpuWindowSub);
		// 	gpuWindowSub = nullptr;
		// }
		break;
	}
	default:
		break;
	}

	return SDL_APP_CONTINUE;
}

void TestGPURendering::Quit(void *appstate) noexcept
{
	gLog = nullptr;
}

void RenderWindow(DrawWindowContentsArgs &args) noexcept
{
	SDL_GPUVertexInputState vertexInputState{
	    .num_vertex_buffers = 0,
	    .num_vertex_attributes = 0,
	};

	SDL_GPUColorTargetDescription colorTargetDescription{
	    .format = SDL_GetGPUSwapchainTextureFormat(device, window),
	    // .blend_state = ,
	};

	SDL_GPUGraphicsPipelineCreateInfo ci{};

	ci.vertex_input_state.num_vertex_attributes = 0;
	ci.vertex_input_state.num_vertex_buffers = 0;

	ci.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
	ci.vertex_shader = shaderVert;
	ci.fragment_shader = shaderFrag;

	ci.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
	ci.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;

	ci.multisample_state.enable_mask = false;
	ci.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;

	ci.target_info.num_color_targets = 1;
	ci.target_info.has_depth_stencil_target = false;

	SDL_GPUColorTargetDescription desc;
	desc.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
	desc.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
	desc.blend_state.color_write_mask = SDL_GPU_COLORCOMPONENT_A | SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G | SDL_GPU_COLORCOMPONENT_B;
	desc.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
	desc.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
	desc.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
	desc.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
	desc.blend_state.enable_blend = true;
	desc.blend_state.enable_color_write_mask = false;
	desc.format = SDL_GetGPUSwapchainTextureFormat(device, window);

	ci.target_info.color_target_descriptions = &desc;

	auto *pipeline = SDL_CreateGPUGraphicsPipeline(device, &ci);

	SDL_BindGPUGraphicsPipeline(args.renderPass, pipeline);

	std::int32_t w, h;
	SDL_GetWindowSize(window, &w, &h);

	SDL_GPUViewport viewport{
	    .w = static_cast<std::float_t>(w),
	    .h = static_cast<std::float_t>(h),
	};
	SDL_SetGPUViewport(args.renderPass, &viewport);

	SDL_DrawGPUPrimitives(args.renderPass, 3, 1, 0, 0);
}
