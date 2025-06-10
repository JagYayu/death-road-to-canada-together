#include "Engine.h"

#include "util/Log.h"

using namespace tudov;

Engine::Engine()
    : _log(Log::Get("Engine")),
      modManager(*this),
      eventManager(*this),
      graphicEngine(*this),
      window(*this)
{
}

void Engine::Run(const MainArgs &args)
{
	_log->Debug("Initializing game engine");
	{
		config.Load();
		window.Initialize();
		eventManager.Initialize();
		modManager.LoadMods();
	}
	_log->Debug("Initialized game engine");

	while (_running)
	{
		window.PoolEvents();

		eventManager.update.Invoke();

		graphicEngine.Render();
	}
}

void Engine::Quit()
{
	_running = false;
}
