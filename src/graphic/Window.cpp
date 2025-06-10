#include "Window.h"

#include <program/Engine.h>

#include <SDL2/SDL.h>

#include <stdexcept>

using namespace tudov;

Window::Window(Engine &engine)
    : engine(engine)
{
}

void Window::Initialize()
{
	auto &&title = engine.config.GetWindowTitle();
	auto &&width = engine.config.GetWindowWidth();
	auto &&height = engine.config.GetWindowHeight();

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		throw std::runtime_error(String("SDL_Init Error: ") + SDL_GetError());
	}

	_window = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	if (!_window)
	{
		SDL_Quit();
		throw std::runtime_error(String("SDL_CreateWindow Error: ") + SDL_GetError());
	}
}

void Window::PoolEvents()
{
	auto &&eventManager = engine.eventManager;

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		// 窗口事件
		case SDL_QUIT:
			engine.Quit();
			break;
		case SDL_WINDOWEVENT:
			break;
		case SDL_KEYDOWN:

			break;
		case SDL_KEYUP:
			break;
		case SDL_TEXTEDITING:
			break;
		case SDL_TEXTINPUT:
			break;

		case SDL_MOUSEMOTION:
			break;
		case SDL_MOUSEBUTTONDOWN:
			break;
		case SDL_MOUSEBUTTONUP:
			break;
		case SDL_MOUSEWHEEL:
			break;

		case SDL_JOYAXISMOTION:
			break;
		case SDL_JOYBALLMOTION:
			break;
		case SDL_JOYHATMOTION:
			break;
		case SDL_JOYBUTTONDOWN:
			break;
		case SDL_JOYBUTTONUP:
			break;
		case SDL_JOYDEVICEADDED:
			break;
		case SDL_JOYDEVICEREMOVED:
			break;
		case SDL_CONTROLLERAXISMOTION:
			break;
		case SDL_CONTROLLERBUTTONDOWN:
			break;
		case SDL_CONTROLLERBUTTONUP:
			break;
		case SDL_CONTROLLERDEVICEADDED:
			break;
		case SDL_CONTROLLERDEVICEREMOVED:
			break;
		case SDL_CONTROLLERDEVICEREMAPPED:
			break;

		case SDL_FINGERDOWN:
			break;
		case SDL_FINGERUP:
			break;
		case SDL_FINGERMOTION:
			break;
		case SDL_MULTIGESTURE:
			break;
		case SDL_DOLLARGESTURE:
			break;
		case SDL_DOLLARRECORD:
			break;

		case SDL_DROPFILE:
			break;
		case SDL_DROPTEXT:
			break;
		case SDL_DROPBEGIN:
			break;
		case SDL_DROPCOMPLETE:
			break;

		case SDL_CLIPBOARDUPDATE:
			break;

		case SDL_APP_TERMINATING:
			break;
		case SDL_APP_LOWMEMORY:
			break;
		case SDL_APP_WILLENTERBACKGROUND:
			break;
		case SDL_APP_DIDENTERBACKGROUND:
			break;
		case SDL_APP_WILLENTERFOREGROUND:
			break;
		case SDL_APP_DIDENTERFOREGROUND:
			break;

		case SDL_USEREVENT:
			break;

		default:
			break;
		}
	}
}
