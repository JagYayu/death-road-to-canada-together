#include "LuaAPI.hpp"

#include "program/Engine.hpp"

#include "sol/property.hpp"

using namespace tudov;

void LuaAPI::RegisterInstallation(const TInstallation &installation) noexcept
{
	_installations.emplace_back(installation);
}

decltype(auto) GetMainWindow(Context &context)
{
	return sol::readonly_property([&]()
	{
		try
		{
			return context.GetEngine().GetMainWindow();
		}
		catch (std::exception &e)
		{
			return std::shared_ptr<IWindow>(nullptr);
		}
	});
}

#define TUDOV_USERTYPE(Class, ...) lua.new_usertype<Class>("tudov_" #Class, __VA_ARGS__)

void LuaAPI::Install(sol::state &lua, Context &context) noexcept
{
	// sol::readonly_property(F &&f)

	TUDOV_USERTYPE(Engine,
	               "mainWindow", GetMainWindow(context),
	               "quit", &Engine::Quit);

	// TUDOV_USERTYPE(ModManager);

	TUDOV_USERTYPE(EventManager,
	               "add", &EventManager::LuaAdd,
	               "new", &EventManager::LuaNew,
	               "invoke", &EventManager::LuaInvoke);

	TUDOV_USERTYPE(Window,
	               "renderer", &Window::renderer,
	               "close", &Window::Close,
	               "shouldClose", &Window::ShouldClose,
	               "getWidth", &Window::GetWidth,
	               "getHeight", &Window::GetHeight,
	               "getSize", &Window::GetSize);

	TUDOV_USERTYPE(Renderer,
	               "clear", &Renderer::LuaClear,
	               "render", &Renderer::Render,
	               "drawRect", &Renderer::LuaDrawTexture,
	               "newRenderTexture", &Renderer::LuaNewRenderTexture,
	               "setRenderTarget", &Renderer::SetRenderTarget,
	               "renderTexture", &Renderer::DrawTexture);

	lua["engine"] = &context.GetEngine();
	lua["fonts"] = &context.GetFontManager();
	lua["images"] = &context.GetImageManager();

	lua["mods"] = std::dynamic_pointer_cast<ModManager>(context.GetModManager().lock());
	lua["events"] = std::dynamic_pointer_cast<EventManager>(context.GetEventManager().lock());
	lua["scriptEngine"] = std::dynamic_pointer_cast<ScriptEngine>(context.GetScriptEngine().lock());
	lua["scriptLoader"] = std::dynamic_pointer_cast<ScriptLoader>(context.GetScriptLoader().lock());
	lua["scriptProvider"] = std::dynamic_pointer_cast<ScriptProvider>(context.GetScriptProvider().lock());

	for (auto &&installation : _installations)
	{
		installation(lua);
	}
}