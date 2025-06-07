#pragma once

#include "GameConfig.h"
#include "IEngine.h"
#include "event/EventManager.h"
#include "mod/ModManager.h"
#include "graphic/GraphicEngine.h"
#include "graphic/Window.h"
#include "util/LogProvider.hpp"

#include <memory>

namespace tudov
{
	class GameEngine : public IEngine, public LogProvider
	{
	private:
		bool running = true;
		
	public:
		EventManager eventManager;
		GameConfig config;
		GraphicEngine graphicEngine;
		ModManager modManager;
		Window window;

		GameEngine();
	
		void Initialize(const MainArgs &args) override;
		void Run() override;
		void Quit() override;
	};
}
