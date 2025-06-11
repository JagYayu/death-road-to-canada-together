#pragma once

#include "EngineConfig.h"
#include "MainArgs.hpp"
#include "event/EventManager.h"
#include "graphic/Window.h"
#include "mod/ModManager.h"

#include <memory>

namespace tudov
{
	class Engine
	{
	  private:
		SharedPtr<Log> _log;
		bool _running = true;

	  public:
		EventManager eventManager;
		EngineConfig config;
		ModManager modManager;
		Window window;

		Engine();

		void Run(const MainArgs &args);
		void Quit();
		
	};
} // namespace tudov
