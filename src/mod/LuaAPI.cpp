#include "LuaAPI.h"

#include "program/Engine.h"

#include "sol/property.hpp"

using namespace tudov;

decltype(auto) GetMainWindow(Context &context)
{
	return sol::readonly_property([&]()
	{
		return context.GetEngine().GetMainWindow();
	});
}

void LuaAPI::Install(sol::state &lua, Context &context) noexcept
{
	// sol::readonly_property(F &&f)

	lua.new_usertype<Engine>("Engine",
	                         "mainWindow", GetMainWindow(context),
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

	lua["engine"] = &context.GetEngine();
	lua["fonts"] = &context.GetFontManager();
	lua["images"] = &context.GetImageManager();
	lua["mods"] = context.GetModManager().lock();
	lua["events"] = context.GetEventManager().lock();
	lua["scriptEngine"] = context.GetScriptEngine().lock();
	lua["scriptLoader"] = context.GetScriptLoader().lock();
	lua["scriptProvider"] = context.GetScriptProvider().lock();
}