/**
 * @file mod/LuaAPI.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "mod/LuaAPI.hpp"

#include "data/VirtualFileSystem.hpp"

#include "graphic/Camera2D.hpp"
#include "graphic/RenderTarget.hpp"
#include "graphic/Renderer.hpp"
#include "i18n/Localization.hpp"
#include "program/Engine.hpp"
#include "program/Window.hpp"
#include "program/WindowManager.hpp"
#include "resource/GlobalResourcesCollection.hpp"
#include "resource/ImageResources.hpp"
#include "system/OperatingSystem.hpp"
#include "system/RandomDevice.hpp"
#include "system/Time.hpp"

#include "sol/forward.hpp"
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
	return sol::readonly_property([&context]()
	{
		try
		{
			return context.GetWindowManager().GetMainWindow();
		}
		catch (std::exception &e)
		{
			return std::shared_ptr<IWindow>(nullptr);
		}
	});
}

#define TE_ENUM(Class, ...)     lua.new_enum<Class>(#Class, __VA_ARGS__)
#define TE_USERTYPE(Class, ...) lua.new_usertype<Class>(#Class, __VA_ARGS__)

void LuaAPI::Install(sol::state &lua, Context &context)
{
	InstallEvent(lua, context);
	InstallMod(lua, context);
	InstallNetwork(lua, context);
	InstallScanCode(lua, context);

	TE_ENUM(ELogVerbosity,
	        {
	            {"All", ELogVerbosity::All},
	            {"None", ELogVerbosity::None},
	            {"Error", ELogVerbosity::Error},
	            {"Warn", ELogVerbosity::Warn},
	            {"Info", ELogVerbosity::Info},
	            {"Debug", ELogVerbosity::Debug},
	            {"Trace", ELogVerbosity::Trace},
	            {"Fatal", ELogVerbosity::Fatal},
	        });

	TE_ENUM(EPathListOption,
	        {
	            {"All", EPathListOption::All},
	            {"None", EPathListOption::None},
	            {"File", EPathListOption::File},
	            {"Directory", EPathListOption::Directory},
	            {"Recursed", EPathListOption::Recursed},
	            {"Sorted", EPathListOption::Sorted},
	            {"FullPath", EPathListOption::FullPath},
	        });

	TE_USERTYPE(Engine,
	            "getVersion", &Engine::LuaGetVersion,
	            "mainWindow", GetMainWindowFromContext(context),
	            "quit", &Engine::Quit,
	            "triggerLoadPending", &Engine::TriggerLoadPending);

	TE_USERTYPE(ImageResources,
	            "getID", &ImageResources::LuaGetID,
	            "getPath", &ImageResources::LuaGetPath);

	TE_USERTYPE(Log,
	            "canOutput", &Log::CanOutput,
	            "output", &Log::LuaOutput);

	TE_USERTYPE(Window,
	            "renderer", &Window::renderer,
	            "close", &Window::Close,
	            "getHeight", &Window::GetHeight,
	            "getKey", &Window::LuaGetKey,
	            "getSize", &Window::GetSize,
	            "getWidth", &Window::GetWidth,
	            "shouldClose", &Window::ShouldClose);

	TE_USERTYPE(Renderer,
	            "beginTarget", &Renderer::LuaBeginTarget,
	            "clear", &Renderer::LuaClear,
	            "draw", &Renderer::LuaDraw,
				"drawDebugText", &Renderer::LuaDrawDebugText,
	            "endTarget", &Renderer::LuaEndTarget,
	            "newRenderTarget", &Renderer::LuaNewRenderTarget,
	            "render", &Renderer::Render);

	TE_USERTYPE(RenderTarget,
	            "getPosition", &RenderTarget::GetCameraPosition,
	            "getHeight", &RenderTarget::GetHeight,
	            "getPositionLerpFactor", &RenderTarget::GetPositionLerpFactor,
	            "getScale", &RenderTarget::GetCameraScale,
	            "getScaleLerpFactor", &RenderTarget::GetScaleLerpFactor,
	            "getSize", &RenderTarget::GetSize,
	            "getCameraTargetScale", &RenderTarget::GetCameraTargetScale,
	            "getCameraTargetScale", &RenderTarget::GetCameraTargetScale,
	            "getWidth", &RenderTarget::GetWidth,
	            "resize", &RenderTarget::Resize,
	            "resizeToFit", &RenderTarget::ResizeToFit,
	            "setPositionLerpFactor", &RenderTarget::SetPositionLerpFactor,
	            "setScaleLerpFactor", &RenderTarget::SetScaleLerpFactor,
	            "setCameraTargetPosition", &RenderTarget::SetCameraTargetPosition,
	            "setCameraTargetScale", &RenderTarget::SetCameraTargetScale,
	            "snapCameraPosition", &RenderTarget::SnapCameraPosition,
	            "snapCameraScale", &RenderTarget::SnapCameraScale,
	            "update", &RenderTarget::Update);

	TE_USERTYPE(VirtualFileSystem,
	            "exists", &VirtualFileSystem::LuaExists,
	            "list", &VirtualFileSystem::LuaList,
	            "readFile", &VirtualFileSystem::LuaReadFile);

	lua.create_named_table(TE_NAMEOF(OperatingSystem),
	                       "getType", &OperatingSystem::GetType,
	                       "isMobile", &OperatingSystem::IsMobile,
	                       "isPC", &OperatingSystem::IsPC);

	lua.create_named_table(TE_NAMEOF(RandomDevice),
	                       "entropy", &RandomDevice::Entropy,
	                       "generate", &RandomDevice::LuaGenerate);

	lua.create_named_table(TE_NAMEOF(Time),
	                       "getStartupTime", &Time::GetStartupTime,
	                       "getSystemTime", &Time::GetSystemTime);

	lua["engine"] = &context.GetEngine();
	lua["localization"] = &dynamic_cast<Localization &>(context.GetLocalization());
	lua["vfs"] = &dynamic_cast<VirtualFileSystem &>(context.GetVirtualFileSystem());

	auto &collection = context.GetGlobalResourcesCollection();
	lua["binaries"] = collection.GetBinariesResources();
	lua["fonts"] = collection.GetFontResources();
	lua["images"] = collection.GetImageResources();
	lua["texts"] = collection.GetTextResources();
}
