#pragma once

#include "ScriptEngine.h"
#include "program/Engine.h"

#include "sol/property.hpp"
#include "sol/sol.hpp"

#include <stdexcept>

namespace tudov
{
	class LuaAPI
	{
	  public:
		inline static void Install(sol::state &lua, Engine *engine) noexcept
		{
			lua.new_usertype<Engine>("Engine",
			                         "mainWindow", &Engine::mainWindow,
			                         "quit", &Engine::Quit);

			lua.new_usertype<ModManager>("ModManager");

			lua.new_usertype<EventManager>("EventManager",
			                               "add", &EventManager::LuaAdd,
			                               "new", &EventManager::LuaNew,
			                               "invoke", &EventManager::LuaInvoke);

			lua.new_usertype<Window>("Window",
			                         "renderer", &Window::renderer,
			                         "close", &Window::Close,
			                         "shouldClose", &Window::ShouldClose,
			                         "getWidth", &Window::GetWidth,
			                         "getHeight", &Window::GetHeight,
			                         "getSize", &Window::GetSize);

			lua.new_usertype<Renderer>("Renderer",
			                           "clear", &Renderer::LuaClear,
			                           "render", &Renderer::Render,
			                           "drawRect", &Renderer::LuaDrawTexture,
			                           "newRenderTexture", &Renderer::LuaNewRenderTexture,
			                           "setRenderTarget", &Renderer::SetRenderTarget,
			                           "renderTexture", &Renderer::DrawTexture);

			lua["engine"] = engine;
			lua["mods"] = &engine->modManager;
			lua["events"] = &engine->modManager.eventManager;
		}
	};
} // namespace tudov
