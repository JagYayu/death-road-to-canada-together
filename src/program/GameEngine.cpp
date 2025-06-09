#include "GameEngine.h"

#include "util/Log.h"

using namespace tudov;

GameEngine::GameEngine()
    : log("GameEngine"),
      eventManager(*this),
      modManager(*this),
      graphicEngine(*this),
      window(*this)
{
}

void GameEngine::Initialize(const MainArgs &args)
{
	log.Debug("Initializing game engine");

	config.Load();
	window.Initialize();

	eventManager.Initialize(modManager.scriptEngine.GetState());

	modManager.LoadMods();

	modManager.scriptLoader.onPreLoadAllScripts += eventManager.ClearScriptHandlers;
	modManager.scriptLoader.onPreLoadAllScripts += eventManager.ClearScriptHandlers;

	log.Debug("Initialized game engine");
}

void GameEngine::Run()
{
	while (running)
	{
		window.PoolEvents();

		// log.Debug("run");
	}
}

void GameEngine::Quit()
{
	running = false;
}
