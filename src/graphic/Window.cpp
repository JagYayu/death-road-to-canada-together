#include "Window.h"

#include "ERenderBackend.h"
#include "mod/ScriptEngine.h"
#include "program/Engine.h"
#include "sdl/SDLRenderer.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>

#include <cmath>
#include <memory>
#include <tuple>

using namespace tudov;

Window::Window(Engine &engine)
    : engine(engine),
      debugManager(*this),
      renderer(),
      _prevTick(),
      _frame(),
      _framerate()
{
	switch (engine.config.GetRenderBackend())
	{
	case tudov::ERenderBackend::SDL:
		renderer = std::make_unique<SDLRenderer>(*this);
		break;
	default:
		throw std::exception("Invalid render backend");
	}
}

Window::~Window() noexcept
{
}

SDL_Window *Window::GetHandle()
{
	return _window;
}

float Window::GetFramerate() const noexcept
{
	return _framerate;
}

void Window::Initialize()
{
	auto &&engineConfig = engine.config;
	auto &&scriptEngine = engine.modManager.scriptEngine;

	auto &&title = engineConfig.GetWindowTitle();
	auto &&width = engineConfig.GetWindowWidth();
	auto &&height = engineConfig.GetWindowHeight();

	_window = SDL_CreateWindow(title.data(), width, height, SDL_WINDOW_RESIZABLE);
	if (!_window)
	{
		SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
	}
	SDL_SetWindowPosition(_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	ImGui::GetIO().DisplaySize = ImVec2(width, height);

	renderer->Initialize();

	InstallToScriptEngine(scriptEngine);
}

void Window::Deinitialize() noexcept
{
}

void Window::InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept
{
	auto &&window = scriptEngine.CreateTable();

	window.set_function("getSize", [this]()
	{
		int w, h;
		SDL_GetWindowSize(_window, &w, &h);
		return std::make_tuple((double)w, (double)h);
	});

	scriptEngine.SetReadonlyGlobal("Window", window);

	renderer->InstallToScriptEngine(scriptEngine);
}

std::tuple<sol::table, EventHandleKey> Window::ResolveKeyEvent(SDL_Event &event) noexcept
{
	auto &&e = engine.modManager.scriptEngine.CreateTable();
	e["key"] = event.key.key;
	e["scancode"] = event.key.scancode;
	e["mod"] = event.key.mod;
	e["repeat"] = event.key.repeat;
	return std::make_tuple(e, EventHandleKey((std::double_t)event.key.key));
}

std::tuple<sol::table, EventHandleKey> Window::ResolveMouseButtonEvent(SDL_Event &event) noexcept
{
	auto &&e = engine.modManager.scriptEngine.CreateTable();
	e["button"] = event.button.button;
	e["x"] = event.button.x;
	e["y"] = event.button.y;
	return std::make_tuple(e, EventHandleKey((std::double_t)event.button.button));
}

void Window::PoolEvents()
{
	auto &&eventManager = engine.modManager.eventManager;
	auto &&scriptEngine = engine.modManager.scriptEngine;

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL3_ProcessEvent(&event);

		switch (event.type)
		{
		case SDL_EVENT_QUIT:
		{
			engine.Quit();
			break;
		}
		case SDL_EVENT_KEY_DOWN:
		{
			auto &&res = ResolveKeyEvent(event);
			eventManager.keyDown->Invoke(std::get<0>(res), std::get<1>(res));
			break;
		}
		case SDL_EVENT_KEY_UP:
		{
			auto &&res = ResolveKeyEvent(event);
			eventManager.keyUp->Invoke(std::get<0>(res), std::get<1>(res));
			break;
		}
		case SDL_EVENT_MOUSE_MOTION:
		{
			auto &&e = scriptEngine.CreateTable();
			e["x"] = event.motion.x;
			e["y"] = event.motion.y;
			e["dx"] = event.motion.xrel;
			e["dy"] = event.motion.yrel;
			eventManager.mouseMove->Invoke(e);
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			auto &&res = ResolveMouseButtonEvent(event);
			eventManager.mouseButtonDown->Invoke(std::get<0>(res), std::get<1>(res));
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			auto &&res = ResolveMouseButtonEvent(event);
			eventManager.mouseButtonUp->Invoke(std::get<0>(res), std::get<1>(res));
			break;
		}
		case SDL_EVENT_MOUSE_WHEEL:
		{
			auto &&e = scriptEngine.CreateTable();
			e["x"] = event.wheel.x;
			e["y"] = event.wheel.y;
			e["direction"] = event.wheel.direction;
			eventManager.mouseWheel->Invoke(e);
			break;
		}
		case SDL_EVENT_TEXT_EDITING:
			break;
		case SDL_EVENT_TEXT_INPUT:
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
		default:
			break;
		}
	}
}

void Window::Render()
{
	auto &&modManager = engine.modManager;
	auto &&scriptLoader = modManager.scriptEngine.scriptLoader;

	++_frame;

	Uint64 target = 1e9 / engine.config.GetWindowFramelimit();
	Uint64 begin = SDL_GetTicksNS();

	renderer->Begin();

	modManager.eventManager.render->Invoke();

	debugManager.UpdateAndRender();

	if (scriptLoader.HasAnyLoadError())
	{
		_errorOverlay.RenderLoadtimeErrors(scriptLoader.GetLoadErrors());
	}

	renderer->End();

	Uint64 delta = SDL_GetTicksNS() - begin;
	if (delta < target)
	{
		SDL_DelayPrecise(target - delta);
	}
	_framerate = double(1e9) / double(SDL_GetTicksNS() - begin);
}
