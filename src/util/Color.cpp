#include "Color.hpp"

#include "SDL3/SDL_pixels.h"

using namespace tudov;

constexpr Color Color::Red = Color(255, 0, 0, 255);

constexpr Color::operator SDL_Color() const noexcept
{
	return SDL_Color{r, g, b, a};
}
