#include "LuaAPI.hpp"

#include "event/EventManager.hpp"
#include "graphic/RenderTarget.hpp"
#include "mod/ModManager.hpp"
#include "program/Engine.hpp"
#include "resource/FontManager.hpp"
#include "resource/ImageManager.hpp"

#include "program/Window.hpp"
#include "sol/property.hpp"

using namespace tudov;

bool LuaAPI::RegisterInstallation(std::string_view name, const TInstallation &installation)
{
	for (auto &&installation : _installations)
	{
		if (std::get<0>(installation) == name) [[unlikely]]
		{
			return false;
		}
	}

	_installations.emplace_back(name, installation);
	return true;
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

#define TE_USERTYPE(Class, ...) lua.new_usertype<Class>("tudov_" #Class, __VA_ARGS__)

void LuaAPI::Install(sol::state &lua, Context &context)
{
	TE_USERTYPE(Engine,
	            "mainWindow", GetMainWindow(context),
	            "quit", &Engine::Quit);

	TE_USERTYPE(EventManager,
	            "add", &EventManager::LuaAdd,
	            "new", &EventManager::LuaNew,
	            "invoke", &EventManager::LuaInvoke);

	TE_USERTYPE(Window,
	            "renderer", &Window::renderer,
	            "close", &Window::Close,
	            "shouldClose", &Window::ShouldClose,
	            "getWidth", &Window::GetWidth,
	            "getHeight", &Window::GetHeight,
	            "getSize", &Window::GetSize);

	TE_USERTYPE(Renderer,
	            "clear", &Renderer::LuaClear,
	            "render", &Renderer::Render,
	            "drawRect", &Renderer::LuaDrawTexture,
	            "newRenderTarget", &Renderer::LuaNewRenderTarget,
	            "renderTexture", &Renderer::DrawTexture);

	TE_USERTYPE(RenderTarget,
	            "beginTarget", &RenderTarget::LuaBeginTarget,
	            "endTarget", &RenderTarget::LuaEndTarget);

	lua["engine"] = &context.GetEngine();
	lua["fonts"] = &context.GetFontManager();
	lua["images"] = &context.GetImageManager();

	lua["mods"] = &dynamic_cast<ModManager &>(context.GetModManager());
	lua["events"] = &dynamic_cast<EventManager &>(context.GetEventManager());
	lua["scriptEngine"] = &dynamic_cast<ScriptEngine &>(context.GetScriptEngine());
	lua["scriptLoader"] = &dynamic_cast<ScriptLoader &>(context.GetScriptLoader());
	lua["scriptProvider"] = &dynamic_cast<ScriptProvider &>(context.GetScriptProvider());
}
