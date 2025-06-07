#include "GameEngine.h"

#include "util/Log.h"

using namespace tudov;

GameEngine::GameEngine()
	: LogProvider("GameEngine"),
	  eventManager(*this),
	  graphicEngine(*this),
	  window(*this)
{
}

void GameEngine::Initialize(const MainArgs &args)
{
	Log::instance.Debug("Initializing game engine");

	config.Load();
	window.Initialize();
	modManager.LoadMods();

	Log::instance.Debug("Initialized game engine");
}

void GameEngine::Run()
{
	while (running)
	{
		window.PoolEvents();
	}
}

void GameEngine::Quit()
{
	running = false;
}
