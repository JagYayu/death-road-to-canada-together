/**
 * @file graphic/Device.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

struct SDL_Device;

namespace tudov
{
	class Device
	{
	  public:
		static void Initialize();
		static void Deinitialize();
		static SDL_Device *Get();
	};
} // namespace tudov
