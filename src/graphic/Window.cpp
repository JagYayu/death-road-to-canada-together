#include "Window.h"

#include <program/GameEngine.h>

#include <SDL2/SDL.h>

#include <stdexcept>

using namespace tudov;

Window::Window(GameEngine &engine)
	: _gameEngine(engine)
{
}

void Window::Initialize()
{
	auto &&title = _gameEngine.config.GetWindowTitle();
	auto &&width = _gameEngine.config.GetWindowWidth();
	auto &&height = _gameEngine.config.GetWindowHeight();

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	if (!_window)
	{
		SDL_Quit();
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}
}

void Window::PoolEvents()
{
	auto &&eventManager = _gameEngine.eventManager;

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			break;

		default:
			break;
		}
	}
}
