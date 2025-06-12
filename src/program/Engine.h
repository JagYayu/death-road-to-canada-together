#pragma once

#include "EngineConfig.h"
#include "MainArgs.hpp"
#include "graphic/Window.h"
#include "mod/ModManager.h"
#include "resource/TextureManager.hpp"
#include "util/Defs.h"

#include <memory>

namespace tudov
{
	class Engine
	{
	  public:
		enum class MountFileType
		{
			Texture,
			Audio,
		};

	  private:
		SharedPtr<Log> _log;
		bool _running = true;

	  public:
		EngineConfig config;
		TextureManager textureManager;
		Window window;
		ModManager modManager;

	  public:
		Engine();

	  private:
		void InitializeResources();

	  public:
		void Run(const MainArgs &args);
		void Quit();
	};
} // namespace tudov
