#pragma once

#include "EngineConfig.h"
#include "MainArgs.hpp"
#include "debug/DebugManager.h"
#include "graphic/Window.h"
#include "mod/ModManager.h"
#include "resource/TextureManager.h"
#include "util/Defs.h"

#include <memory>

namespace tudov
{
	class Engine
	{
	  private:
		std::shared_ptr<Log> _log;
		bool _running = true;

	  public:
		EngineConfig config;
		Window window;
		ModManager modManager;
		TextureManager textureManager;

	  public:
		Engine();

	  private:
		void InitializeResources();

	  public:
		void Run(const MainArgs &args);
		void Quit();
	};
} // namespace tudov
