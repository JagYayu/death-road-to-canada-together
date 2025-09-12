/**
 * @file graphic/BlendMode.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "SDL3/SDL_blendmode.h"

#include <cstdint>

namespace tudov
{
	enum class EBlendMode : std::uint8_t
	{
		None = SDL_BLENDMODE_NONE,
		Blend = SDL_BLENDMODE_BLEND,
		Add = SDL_BLENDMODE_ADD,
		Mod = SDL_BLENDMODE_MOD,
		Mul = SDL_BLENDMODE_MUL,
	};
} // namespace tudov
