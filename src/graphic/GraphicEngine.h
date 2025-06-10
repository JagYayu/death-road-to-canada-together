#pragma once

#include "ERenderBackend.h"

#include <memory>
#include <thread>

namespace tudov
{
	class Engine;
	class Window;

	class GraphicEngine
	{
	  private:
		ERenderBackend _renderBackend;
		std::thread _renderThread;
		bool _renderThreadRunning;

	  public:
		Engine &engine;

	  public:
		explicit GraphicEngine(Engine &engine);

	  private:
		void RenderThread();

	  public:
		void GetRenderBackend(ERenderBackend backend);
		void SetRenderBackend(ERenderBackend backend);

		void Init(Window &window);
		void Shutdown();
		void Render();
		void BeginFrame();
		void EndFrame();
	};
} // namespace tudov