#include "Window.h"

#include "imgui_impl_sdl3.h"
#include "program/Engine.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_video.h>
#include <imgui.h>

using namespace tudov;

Window::Window(Engine &engine)
    : engine(engine),
      debugManager(*this),
      renderer(*this)
{
}

SDL_Window *Window::GetHandle()
{
	return _window;
}

void Window::Initialize()
{
	auto &&title = engine.config.GetWindowTitle();
	auto &&width = engine.config.GetWindowWidth();
	auto &&height = engine.config.GetWindowHeight();

	_window = SDL_CreateWindow(title.data(), width, height, SDL_WINDOW_RESIZABLE);
	if (!_window)
	{
		SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
	}
	SDL_SetWindowPosition(_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	ImGui::GetIO().DisplaySize = ImVec2(width, height);

	renderer.Initialize();
}

void Window::PoolEvents()
{
	auto &&eventManager = engine.modManager.eventManager;

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		ImGui_ImplSDL3_ProcessEvent(&e);

		switch (e.type)
		{
		// 窗口事件
		case SDL_EVENT_QUIT:
			engine.Quit();
			break;
		case SDL_EVENT_KEY_DOWN:

			break;
		case SDL_EVENT_KEY_UP:
			break;
		case SDL_EVENT_TEXT_EDITING:
			break;
		case SDL_EVENT_TEXT_INPUT:
			break;

		case SDL_EVENT_MOUSE_MOTION:
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			break;
		case SDL_EVENT_MOUSE_WHEEL:
			break;

		case SDL_EVENT_JOYSTICK_AXIS_MOTION:
			break;
		case SDL_EVENT_JOYSTICK_BALL_MOTION:
			break;
		case SDL_EVENT_JOYSTICK_HAT_MOTION:
			break;
		case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
			break;
		case SDL_EVENT_JOYSTICK_BUTTON_UP:
			break;
			// case SDL_EVENT_JOYDEVICEADDED:
			// 	break;
			// case SDL_EVENT_JOYDEVICEREMOVED:
			// 	break;
			// case SDL_EVENT_CONTROLLERAXISMOTION:
			// 	break;
			// case SDL_EVENT_CONTROLLERBUTTONDOWN:
			// 	break;
			// case SDL_EVENT_CONTROLLERBUTTONUP:
			// 	break;
			// case SDL_EVENT_CONTROLLERDEVICEADDED:
			// 	break;
			// case SDL_EVENT_CONTROLLERDEVICEREMOVED:
			// 	break;
			// case SDL_EVENT_CONTROLLERDEVICEREMAPPED:
			// 	break;

			// case SDL_EVENT_FINGERDOWN:
			// 	break;
			// case SDL_EVENT_FINGERUP:
			// 	break;
			// case SDL_EVENT_FINGERMOTION:
			// 	break;
			// case SDL_EVENT_MULTIGESTURE:
			// 	break;
			// case SDL_EVENT_DOLLARGESTURE:
			// 	break;
			// case SDL_EVENT_DOLLARRECORD:
			// 	break;

			// case SDL_EVENT_DROPFILE:
			// 	break;
			// case SDL_EVENT_DROPTEXT:
			// 	break;
			// case SDL_EVENT_DROPBEGIN:
			// 	break;
			// case SDL_EVENT_DROPCOMPLETE:
			// 	break;

			// case SDL_EVENT_CLIPBOARDUPDATE:
			// 	break;

			// case SDL_EVENT_APP_TERMINATING:
			// 	break;
			// case SDL_EVENT_APP_LOWMEMORY:
			// 	break;
			// case SDL_EVENT_APP_WILLENTERBACKGROUND:
			// 	break;
			// case SDL_EVENT_APP_DIDENTERBACKGROUND:
			// 	break;
			// case SDL_EVENT_APP_WILLENTERFOREGROUND:
			// 	break;
			// case SDL_EVENT_APP_DIDENTERFOREGROUND:
			// 	break;

			// case SDL_EVENT_USEREVENT:
			// 	break;

		default:
			break;
		}
	}
}

void Window::Render()
{
	renderer.Begin();
	engine.modManager.eventManager.render->Invoke();
	debugManager.UpdateAndRender();
	renderer.End();
}
