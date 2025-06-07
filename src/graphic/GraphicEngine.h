#pragma once

#include "ERenderBackend.h"

#include <memory>

namespace tudov
{
	class GameEngine;
	
	class GraphicEngine
	{
	private:
		GameEngine& _gameEngine;
		ERenderBackend _renderBackend;

	public:
		explicit GraphicEngine(GameEngine& gameEngine);

		void GetRenderBackend(ERenderBackend backend);
		void SetRenderBackend(ERenderBackend backend);

		void Shutdown();
		void BeginFrame();
		void EndFrame();
	};
}