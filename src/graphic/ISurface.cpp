#include "ISurface.h"

#include "IRenderer.h"

using namespace tudov;

ISurface::ISurface(IRenderer &renderer) noexcept
    : renderer(renderer)
{
}

glm::vec2 ISurface::GetSize() const noexcept
{
	return {GetWidth(), GetHeight()};
}
