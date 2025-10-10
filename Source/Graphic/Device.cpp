/**
 * @file graphic/Device.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "graphic/Device.hpp"

// #include "System/Log.hpp"

// #include "SDL3/SDL_error.h"
// #include "SDL3/SDL_gpu.h"

// #include <stdexcept>

using namespace tudov;

static SDL_Device *gpuDevice = nullptr;

void Device::Initialize()
{
	// auto log = *Log::Get("Device");

	// gpuDevice = SDL_CreateDevice(SDL_GPU_SHADERFORMAT_PRIVATE |
	//                                     SDL_GPU_SHADERFORMAT_SPIRV |
	//                                     SDL_GPU_SHADERFORMAT_DXBC |
	//                                     SDL_GPU_SHADERFORMAT_DXIL |
	//                                     SDL_GPU_SHADERFORMAT_MSL |
	//                                     SDL_GPU_SHADERFORMAT_METALLIB,
	//                                 true, "vulkan");
	// if (gpuDevice == nullptr)
	// {
	// 	log.Fatal("Failed to initialize gpu device: {}", SDL_GetError());
	// }
	// else
	// {
	// 	log.Debug("Initialized gpu device");
	// }

	// if (log.CanDebug())
	// {
	// 	log.Debug("Listing available gpu drivers ...");
	// 	for (std::int32_t index = 0; index < SDL_GetNumGPUDrivers(); ++index)
	// 	{
	// 		log.Debug("{}. {}", index + 1, SDL_GetGPUDriver(index));
	// 	}
	// }
}

void Device::Deinitialize()
{
	// if (gpuDevice != nullptr)
	// {
	// 	SDL_DestroyDevice(gpuDevice);
	// 	gpuDevice = nullptr;
	// }
}

SDL_Device *Device::Get()
{
	// if (gpuDevice == nullptr) [[unlikely]]
	// {
	// 	throw std::runtime_error("not uninitialized!");
	// }
	// return gpuDevice;
	return nullptr;
}
