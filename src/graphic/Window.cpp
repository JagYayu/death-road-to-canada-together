#include "Window.h"

#include "SDL3/SDL_render.h"
#include "graphic/sdl/SDLRenderer.h"
#include "mod/ScriptEngine.h"
#include "program/Engine.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <cmath>
#include <memory>
#include <tuple>

using namespace tudov;

Window::Window(Engine &engine)
    : engine(engine),
      renderer(*this),
      debugManager(*this),
      _initialized(false),
      _prevTick(),
      _frame(),
      _framerate()
{
}

Window::~Window() noexcept
{
	SDL_DestroyWindow(_window);
}

SDL_Window *Window::GetHandle()
{
	return _window;
}

float Window::GetFramerate() const noexcept
{
	return _framerate;
}

std::tuple<std::int32_t, std::int32_t> Window::GetSize() const noexcept
{
	int w, h;
	SDL_GetWindowSize(_window, &w, &h);
	return {w, h};
}

void Window::Initialize()
{
	if (_initialized)
	{
		return;
	}

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

	_renderer = SDL_CreateRenderer(_window, nullptr);

	if (!ImGui_ImplSDL3_InitForSDLRenderer(_window, _renderer))
	{
		SDL_Log("ImGui_ImplSDL3_InitForSDLRenderer failed: %s", SDL_GetError());
	}
	if (!ImGui_ImplSDLRenderer3_Init(_renderer))
	{
		SDL_Log("ImGui_ImplSDLRenderer3_Init failed: %s", SDL_GetError());
	}

	renderer.Initialize();

	ImGui::GetIO().DisplaySize = ImVec2(width, height);

	InstallToScriptEngine(scriptEngine);

	_initialized = true;
}

void Window::Deinitialize() noexcept
{
}

void Window::InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept
{
	// auto &&window = scriptEngine.CreateTable();

	// window.set_function("getSize", [this]()
	// {
	// 	int w, h;
	// 	SDL_GetWindowSize(_window, &w, &h);
	// 	return std::make_tuple((double)w, (double)h);
	// });

	// window.set_function("newRenderer", [this]()
	// {
	// 	// switch (engine.config.GetRenderBackend())
	// 	// auto &&sdlRenderer = std::make_unique<SDLRenderer>(*this);
	// 	return std::make_shared<SDLRenderer>(*this);
	// });

	// scriptEngine.SetReadonlyGlobal("Window", window);
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

void Window::PollEvents()
{
	if (!_initialized)
	{
		Initialize();
	}

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
	auto &&renderBackend = engine.config.GetRenderBackend();

	++_frame;

	Uint64 target = 1e9 / engine.config.GetWindowFramelimit();
	Uint64 begin = SDL_GetTicksNS();

	renderer.Begin();
	// {
	// 	auto &&io = ImGui::GetIO();
	// 	auto &&raw = dynamic_cast<SDLRenderer *>(renderer.get())->GetRaw();
	// 	SDL_SetRenderScale(raw, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
	// }

	modManager.eventManager.render->Invoke();

	debugManager.UpdateAndRender();

	if (scriptLoader.HasAnyLoadError())
	{
		_errorOverlay.RenderLoadtimeErrors(scriptLoader.GetLoadErrors());
	}

	// {
	// 	auto &&raw = dynamic_cast<SDLRenderer *>(renderer.get())->GetRaw();
	// 	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), raw);
	// }
	renderer.End();

	Uint64 delta = SDL_GetTicksNS() - begin;
	if (delta < target)
	{
		// SDL_DelayPrecise(target - delta);
	}
	_framerate = double(1e9) / double(SDL_GetTicksNS() - begin);
}
