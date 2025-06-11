#include "Engine.h"

#include "util/Log.h"

using namespace tudov;

Engine::Engine()
    : _log(Log::Get("Engine")),
      modManager(*this),
      eventManager(*this),
      window(*this)
{
}

void Engine::Run(const MainArgs &args)
{
	SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);

	_log->Debug("Initializing game engine");
	{
		config.Load();
		window.Initialize();
		eventManager.Initialize();
		modManager.Initialize();
		modManager.LoadMods();
	}
	_log->Debug("Initialized game engine");

	while (_running)
	{
		window.PoolEvents();

		eventManager.update.Invoke();

		window.Render();
	}
}

void Engine::Quit()
{
	_running = false;
}
