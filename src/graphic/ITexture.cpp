#include "ITexture.h"

#include "IRenderer.h"

using namespace tudov;

ITexture::ITexture(IRenderer &renderer) noexcept
    : renderer(renderer)
{
}
