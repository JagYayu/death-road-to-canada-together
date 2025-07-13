#include "LuaAPI.hpp"

#include "program/Engine.hpp"

#include "sol/property.hpp"
#include <tuple>

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

#define TUDOV_USERTYPE(Class, ...) lua.new_usertype<Class>("tudov_" #Class, __VA_ARGS__)

void LuaAPI::Install(sol::state &lua, Context &context)
{
	// sol::readonly_property(F &&f)

	// TUDOV_USERTYPE(Engine,
	//                "mainWindow", GetMainWindow(context),
	//                "quit", &Engine::Quit);

	// TUDOV_USERTYPE(ModManager);

	TUDOV_USERTYPE(EventManager,
	               "add", &EventManager::LuaAdd,
	               "new", &EventManager::LuaNew,
	               "invoke", &EventManager::LuaInvoke);

	// TUDOV_USERTYPE(Window,
	//                "renderer", &Window::renderer,
	//                "close", &Window::Close,
	//                "shouldClose", &Window::ShouldClose,
	//                "getWidth", &Window::GetWidth,
	//                "getHeight", &Window::GetHeight,
	//                "getSize", &Window::GetSize);

	// TUDOV_USERTYPE(Renderer,
	//                "clear", &Renderer::LuaClear,
	//                "render", &Renderer::Render,
	//                "drawRect", &Renderer::LuaDrawTexture,
	//                "newRenderTexture", &Renderer::LuaNewRenderTexture,
	//                "setRenderTexture", &Renderer::SetRenderTexture,
	//                "renderTexture", &Renderer::DrawTexture);

	// lua["engine"] = &context.GetEngine();
	lua["fonts"] = &context.GetFontManager();
	lua["images"] = &context.GetImageManager();

	lua["mods"] = dynamic_cast<ModManager *>(context.GetModManager());
	lua["events"] = dynamic_cast<EventManager *>(context.GetEventManager());
	lua["scriptEngine"] = dynamic_cast<ScriptEngine *>(context.GetScriptEngine());
	lua["scriptLoader"] = dynamic_cast<ScriptLoader *>(context.GetScriptLoader());
	lua["scriptProvider"] = dynamic_cast<ScriptProvider *>(context.GetScriptProvider());

	for (auto &&installation : _installations)
	{
		std::get<1>(installation)(lua);
	}
}