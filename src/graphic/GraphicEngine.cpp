#include "GraphicEngine.h"

#include "Window.h"
#include "program/Engine.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

using namespace tudov;

GraphicEngine::GraphicEngine(Engine &engine)
    : engine(engine)
{
}

void GraphicEngine::GetRenderBackend(ERenderBackend backend)
{
}

void GraphicEngine::SetRenderBackend(ERenderBackend backend)
{
}

void GraphicEngine::Init(Window &window)
{
	bgfx::Init init;
	init.type = bgfx::RendererType::Enum::Count;
	init.vendorId = BGFX_PCI_ID_NONE;
	init.resolution.width = engine.config.GetWindowWidth();
	init.resolution.height = engine.config.GetWindowHeight();
	init.resolution.reset = BGFX_RESET_VSYNC;

	if (!bgfx::init(init))
	{
		throw std::runtime_error("Failed to initialize bgfx!");
	}

	_renderThreadRunning = true;
	_renderThread = std::thread(&GraphicEngine::RenderThread, this);
}

void GraphicEngine::Shutdown()
{
	bgfx::shutdown();
}

void GraphicEngine::Render()
{
	engine.eventManager.render.Invoke();

	bgfx::touch(0);
	bgfx::frame();
}

void GraphicEngine::RenderThread()
{
	while (_renderThreadRunning)
	{
		bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
		bgfx::setViewRect(0, 0, 0, 1920, 1080);

		bgfx::frame();

		std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
	}
}

void GraphicEngine::BeginFrame()
{
}

void GraphicEngine::EndFrame()
{
}
