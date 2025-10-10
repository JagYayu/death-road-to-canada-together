/**
 * @file util/Color.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Util/Color.hpp"

#include "SDL3/SDL_pixels.h"

using namespace tudov;

constexpr Color Color::White = Color(255, 255, 255, 255);
constexpr Color Color::Red = Color(255, 0, 0, 255);
constexpr Color Color::Green = Color(0, 255, 0, 255);
constexpr Color Color::Blue = Color(0, 0, 255, 255);
constexpr Color Color::Black = Color(0, 0, 0, 255);
constexpr Color Color::Transparent = Color(0, 0, 0, 0);

Color::operator SDL_Color() const noexcept
{
	return SDL_Color{r, g, b, a};
}

Color::operator SDL_FColor() const noexcept
{
	return SDL_FColor{r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
}
