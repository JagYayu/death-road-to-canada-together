/**
 * @file program/MouseButton.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "util/Micros.hpp"
#include "util/Utils.hpp"

#include "SDL3/SDL_mouse.h"

#include <cstdint>

namespace tudov
{
	enum class EMouseButton : std::int8_t
	{
		None = 0,
		Left = SDL_BUTTON_LEFT,
		Middle = SDL_BUTTON_MIDDLE,
		Right = SDL_BUTTON_RIGHT,
		Extra1 = SDL_BUTTON_X1,
		Extra2 = SDL_BUTTON_X2,
	};

	enum class EMouseButtonFlag : std::int8_t
	{
		Left = SDL_BUTTON_LMASK,
		Middle = SDL_BUTTON_MMASK,
		Right = SDL_BUTTON_RMASK,
		Extra1 = SDL_BUTTON_X1MASK,
		Extra2 = SDL_BUTTON_X2MASK,
	};

	struct MouseButton
	{
		TE_STATIC_CLASS(MouseButton);

		TE_FORCEINLINE EMouseButtonFlag ToFlag(EMouseButton button) const noexcept
		{
			return static_cast<EMouseButtonFlag>(SDL_BUTTON_MASK(Utils::ToUnderlying(button)));
		}

		TE_FORCEINLINE EMouseButton FromFlag(EMouseButtonFlag buttonFlag) const noexcept
		{
			switch (Utils::ToUnderlying(buttonFlag))
			{
			case SDL_BUTTON_LMASK:
				return EMouseButton::Left;
			case SDL_BUTTON_MMASK:
				return EMouseButton::Middle;
			case SDL_BUTTON_RMASK:
				return EMouseButton::Right;
			case SDL_BUTTON_X1MASK:
				return EMouseButton::Extra1;
			case SDL_BUTTON_X2MASK:
				return EMouseButton::Extra2;
			default:
				return EMouseButton::None;
			}
		}
	};
} // namespace tudov
