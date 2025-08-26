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
#include "event/EventInvocation.hpp"
#include "event/EventManager.hpp"
#include "graphic/Camera2D.hpp"
#include "graphic/RenderTarget.hpp"
#include "graphic/Renderer.hpp"
#include "i18n/Localization.hpp"
#include "mod/ModManager.hpp"
#include "mod/ScriptLoader.hpp"
#include "network/ClientSession.hpp"
#include "network/NetworkManager.hpp"
#include "network/ServerSession.hpp"
#include "program/Engine.hpp"
#include "program/Window.hpp"
#include "program/WindowManager.hpp"
#include "resource/GlobalResourcesCollection.hpp"
#include "resource/ImageResources.hpp"
#include "system/RandomDevice.hpp"
#include "system/Time.hpp"
#include "util/LuaUtils.hpp"

#include "sol/forward.hpp"
#include "sol/property.hpp"
#include "sol/string_view.hpp"

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
	InstallNetwork(lua, context);
	InstallScanCode(lua, context);

	TE_ENUM(EEventInvocation,
	        {
	            {"All", EEventInvocation::All},
	            {"None", EEventInvocation::None},
	            {"CacheHandlers", EEventInvocation::CacheHandlers},
	            {"NoProfiler", EEventInvocation::NoProfiler},
	            {"TrackProgression", EEventInvocation::TrackProgression},
	            {"Default", EEventInvocation::Default},
	        });

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
	            "mainWindow", GetMainWindowFromContext(context),
	            "quit", &Engine::Quit,
	            "triggerLoadPending", &Engine::TriggerLoadPending);

	TE_USERTYPE(EventManager,
	            "add", &EventManager::LuaAdd,
	            "new", &EventManager::LuaNew,
	            "invoke", &EventManager::LuaInvoke);

	TE_USERTYPE(ImageResources,
	            "getID", &ImageResources::LuaGetID,
	            "getPath", &ImageResources::LuaGetPath);

	TE_USERTYPE(Log,
	            "canOutput", &Log::CanOutput,
	            "output", &Log::Output);

	TE_USERTYPE(NetworkManager,
	            "getClient", &NetworkManager::LuaGetClient,
	            "getServer", &NetworkManager::LuaGetServer);

	TE_USERTYPE(ModConfig,
	            "author", &ModConfig::author,
	            "description", &ModConfig::description,
	            "distribution", &ModConfig::distribution,
	            "name", &ModConfig::name,
	            "namespace", &ModConfig::namespace_,
	            "uid", &ModConfig::uid);

	TE_USERTYPE(ModManager,
	            "loadMods", &ModManager::LoadModsDeferred,
	            "unloadMods", &ModManager::UnloadModsDeferred);

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

	TE_USERTYPE(ScriptLoader,
	            "addReverseDependency", &ScriptLoader::LuaAddReverseDependency,
	            "getLoadingScriptID", &ScriptLoader::GetLoadingScriptID,
	            "getLoadingScriptName", &ScriptLoader::GetLoadingScriptName);

	TE_USERTYPE(ScriptProvider,
	            "getScriptTextID", &ScriptProvider::GetScriptTextID);

	TE_USERTYPE(VirtualFileSystem,
	            "exists", &VirtualFileSystem::LuaExists,
	            "list", &VirtualFileSystem::LuaList,
	            "readFile", &VirtualFileSystem::LuaReadFile);

	// TE_USERTYPE(Version,
	//             "major", &Version::Major,
	//             "minor", &Version::Minor,
	//             "parts", &Version::_parts,
	//             "patch", &Version::Patch);

	lua.create_named_table(TE_NAMEOF(RandomDevice),
	                       "entropy", &RandomDevice::Entropy,
	                       "generate", &RandomDevice::LuaGenerate);

	lua.create_named_table(TE_NAMEOF(Time),
	                       "getSystemTime", &Time::GetSystemTime);

	lua["engine"] = &context.GetEngine();
	lua["events"] = &dynamic_cast<EventManager &>(context.GetEventManager());
	lua["localization"] = &dynamic_cast<Localization &>(context.GetLocalization());
	lua["mods"] = &dynamic_cast<ModManager &>(context.GetModManager());
	lua["network"] = &dynamic_cast<NetworkManager &>(context.GetNetworkManager());
	lua["scriptLoader"] = &dynamic_cast<ScriptLoader &>(context.GetScriptLoader());
	lua["scriptProvider"] = &dynamic_cast<ScriptProvider &>(context.GetScriptProvider());
	lua["vfs"] = &dynamic_cast<VirtualFileSystem &>(context.GetVirtualFileSystem());

	auto &collection = context.GetGlobalResourcesCollection();
	lua["binaries"] = collection.GetBinariesResources();
	lua["fonts"] = collection.GetFontResources();
	lua["images"] = collection.GetImageResources();
	lua["texts"] = collection.GetTextResources();

	lua.set_function("getModConfig", [this, &context](sol::string_view modUID) -> ModConfig *
	{
		std::shared_ptr<Mod> mod = context.GetModManager().FindLoadedMod(modUID);
		return mod == nullptr ? nullptr : &mod->GetConfig();
	});
}
