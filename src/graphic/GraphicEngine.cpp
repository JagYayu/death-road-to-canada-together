#include "GraphicEngine.h"

#include <bgfx/bgfx.h>

using namespace tudov;

GraphicEngine::GraphicEngine(GameEngine &gameEngine)
	: _gameEngine(gameEngine)
{
}

void tudov::GraphicEngine::GetRenderBackend(ERenderBackend backend)
{
}

void tudov::GraphicEngine::SetRenderBackend(ERenderBackend backend)
{
}

void tudov::GraphicEngine::Shutdown()
{
}

void tudov::GraphicEngine::BeginFrame()
{
}

void tudov::GraphicEngine::EndFrame()
{
}
