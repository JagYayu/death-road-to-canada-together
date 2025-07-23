#include "mod/LuaAPI.hpp"

#include "event/EventManager.hpp"
#include "graphic/Camera2D.hpp"
#include "graphic/RenderTarget.hpp"
#include "mod/ModManager.hpp"
#include "mod/ScriptLoader.hpp"
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

decltype(auto) GetMainWindowFromContext(Context &context)
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

#define TE_ENGINE_CONTEXT_GETTER(what)                 \
	decltype(auto) what##FromContext(Context &context) \
	{                                                  \
		return sol::readonly_property([&]()            \
		{                                              \
			return &context.what();                    \
		});                                            \
	}

#define TE_USERTYPE(Class, ...) lua.new_usertype<Class>("tudov_" #Class, __VA_ARGS__)

void LuaAPI::Install(sol::state &lua, Context &context)
{
	TE_USERTYPE(Camera2D,
	            "getPosition", &Camera2D::GetPosition,
	            "getPositionLerpFactor", &Camera2D::GetPositionLerpFactor,
	            "getScale", &Camera2D::GetScale,
	            "getScaleLerpFactor", &Camera2D::GetScaleLerpFactor,
	            "getTargetScale", &Camera2D::GetTargetScale,
	            "getTargetScale", &Camera2D::GetTargetScale,
	            "getTargetViewSize", &Camera2D::GetTargetViewSize,
	            "getViewLerpFactor", &Camera2D::GetViewLerpFactor,
	            "getViewScaleMode", &Camera2D::GetViewScaleMode,
	            "getViewSize", &Camera2D::GetViewSize,
	            "setPositionLerpFactor", &Camera2D::SetPositionLerpFactor,
	            "setScaleLerpFactor", &Camera2D::SetScaleLerpFactor,
	            "setTargetPosition", &Camera2D::SetTargetPosition,
	            "setTargetScale", &Camera2D::SetTargetScale,
	            "setTargetViewSize", &Camera2D::SetTargetViewSize,
	            "setViewLerpFactor", &Camera2D::SetViewLerpFactor,
	            "setViewScaleMode", &Camera2D::SetViewScaleMode);

	TE_USERTYPE(Engine,
	            "mainWindow", GetMainWindowFromContext(context),
	            "quit", &Engine::Quit);

	TE_USERTYPE(EventManager,
	            "add", &EventManager::LuaAdd,
	            "new", &EventManager::LuaNew,
	            "invoke", &EventManager::LuaInvoke);

	TE_USERTYPE(ImageManager,
	            "getID", &ImageManager::LuaGetID,
	            "getPath", &ImageManager::LuaGetPath);

	TE_USERTYPE(Window,
	            "renderer", &Window::renderer,
	            "close", &Window::Close,
	            "shouldClose", &Window::ShouldClose,
	            "getWidth", &Window::GetWidth,
	            "getHeight", &Window::GetHeight,
	            "getSize", &Window::GetSize);

	TE_USERTYPE(Renderer,
	            "beginTarget", &Renderer::LuaBeginTarget,
	            "clear", &Renderer::LuaClear,
	            "draw", &Renderer::LuaDraw,
	            "endTarget", &Renderer::LuaEndTarget,
	            "getTargetSize", &Renderer::LuaGetTargetSize,
	            "newRenderTarget", &Renderer::LuaNewRenderTarget,
	            "render", &Renderer::Render);

	TE_USERTYPE(RenderTarget,
	            "getPosition", &RenderTarget::GetPosition,
	            "getHeight", &RenderTarget::GetHeight,
	            "getPositionLerpFactor", &RenderTarget::GetPositionLerpFactor,
	            "getScale", &RenderTarget::GetScale,
	            "getScaleLerpFactor", &RenderTarget::GetScaleLerpFactor,
	            "getSize", &RenderTarget::GetSize,
	            "getTargetScale", &RenderTarget::GetTargetScale,
	            "getTargetScale", &RenderTarget::GetTargetScale,
	            "getTargetViewSize", &RenderTarget::GetTargetViewSize,
	            "getViewLerpFactor", &RenderTarget::GetViewLerpFactor,
	            "getViewScaleMode", &RenderTarget::GetViewScaleMode,
	            "getViewSize", &RenderTarget::GetViewSize,
	            "getWidth", &RenderTarget::GetWidth,
	            "resize", &RenderTarget::Resize,
	            "resizeToFit", &RenderTarget::ResizeToFit,
	            "setPositionLerpFactor", &RenderTarget::SetPositionLerpFactor,
	            "setScaleLerpFactor", &RenderTarget::SetScaleLerpFactor,
	            "setTargetPosition", &RenderTarget::SetTargetPosition,
	            "setTargetScale", &RenderTarget::SetTargetScale,
	            "setTargetViewSize", &RenderTarget::SetTargetViewSize,
	            "setViewLerpFactor", &RenderTarget::SetViewLerpFactor,
	            "setViewScaleMode", &RenderTarget::SetViewScaleMode,
	            "update", &RenderTarget::Update);

	lua["engine"] = &context.GetEngine();
	lua["fonts"] = &context.GetFontManager();
	lua["images"] = &context.GetImageManager();

	lua["mods"] = &dynamic_cast<ModManager &>(context.GetModManager());
	lua["events"] = &dynamic_cast<EventManager &>(context.GetEventManager());
	lua["scriptEngine"] = &dynamic_cast<ScriptEngine &>(context.GetScriptEngine());
	lua["scriptLoader"] = &dynamic_cast<ScriptLoader &>(context.GetScriptLoader());
	lua["scriptProvider"] = &dynamic_cast<ScriptProvider &>(context.GetScriptProvider());
}
