#include "IRenderBuffer.h"

#include "graphic/IRenderer.h"

using namespace tudov;

IRenderBuffer::IRenderBuffer(IRenderer &renderer) noexcept
    : renderer(renderer)
{
}
