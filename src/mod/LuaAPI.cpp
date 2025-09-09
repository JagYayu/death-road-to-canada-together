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
#include "debug/Debug.hpp"
#include "graphic/Camera2D.hpp"
#include "graphic/RenderArgs.hpp"
#include "graphic/RenderBuffer.hpp"
#include "graphic/RenderTarget.hpp"
#include "graphic/Renderer.hpp"
#include "i18n/Localization.hpp"
#include "math/Geometry.hpp"
#include "program/Engine.hpp"
#include "program/Window.hpp"
#include "program/WindowManager.hpp"
#include "resource/FontResources.hpp"
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
#define TE_CLASS(Class, ...)    lua.create_named_table(("" #Class ""), __VA_ARGS__)

void LuaAPI::Install(sol::state &lua, Context &context)
{
	InstallEvent(lua, context);
	InstallMod(lua, context);
	InstallNetwork(lua, context);
	InstallScanCode(lua, context);

	TE_ENUM(
	    EDebugConsoleCode,
	    {
	        {"None", EDebugConsoleCode::None},
	        {"Failure", EDebugConsoleCode::Failure},
	        {"Success", EDebugConsoleCode::Success},
	        {"Warn", EDebugConsoleCode::Warn},
	    });

	TE_ENUM(
	    ELogVerbosity,
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

	TE_ENUM(
	    EOperatingSystem,
	    {
	        {"Unknown", EOperatingSystem::Unknown},
	        {"Windows", EOperatingSystem::Windows},
	        {"Linux", EOperatingSystem::Linux},
	        {"MaxOS", EOperatingSystem::MaxOS},
	        {"Android", EOperatingSystem::Android},
	        {"IOS", EOperatingSystem::IOS},
	    });

	TE_ENUM(
	    EPathListOption,
	    {
	        {"All", EPathListOption::All},
	        {"None", EPathListOption::None},
	        {"File", EPathListOption::File},
	        {"Directory", EPathListOption::Directory},
	        {"Recursed", EPathListOption::Recursed},
	        {"Sorted", EPathListOption::Sorted},
	        {"FullPath", EPathListOption::FullPath},
	    });

	TE_USERTYPE(
	    DrawRectArgs,
	    sol::call_constructor, sol::constructors<DrawRectArgs()>(),
	    "angle", sol::property(&DrawRectArgs::LuaGetAngle, &DrawRectArgs::LuaSetAngle),
	    "color", sol::property(&DrawRectArgs::LuaGetColor, &DrawRectArgs::LuaSetColor),
	    "destination", sol::property(&DrawRectArgs::LuaGetDestination, &DrawRectArgs::LuaSetDestination),
	    // "origin", sol::property(&DrawRectArgs::LuaGetOrigin, &DrawRectArgs::LuaSetOrigin),
	    "source", sol::property(&DrawRectArgs::LuaGetSource, &DrawRectArgs::LuaSetSource),
	    "texture", sol::property(&DrawRectArgs::LuaGetTexture, &DrawRectArgs::LuaSetTexture));

	TE_USERTYPE(
	    DrawTextArgs,
	    sol::call_constructor, sol::constructors<DrawTextArgs()>(),
	    "alignX", sol::property(&DrawTextArgs::GetAlignX, &DrawTextArgs::SetAlignX),
	    "alignY", sol::property(&DrawTextArgs::GetAlignY, &DrawTextArgs::SetAlignY),
	    "characterScale", sol::property(&DrawTextArgs::GetCharacterScale, &DrawTextArgs::SetCharacterScale),
	    "font", sol::property(&DrawTextArgs::GetFont, &DrawTextArgs::SetFont),
	    "maxWidth", sol::property(&DrawTextArgs::GetMaxWidth, &DrawTextArgs::SetMaxWidth),
	    "scale", sol::property(&DrawTextArgs::GetScale, &DrawTextArgs::SetScale),
	    "text", sol::property(&DrawTextArgs::GetText, &DrawTextArgs::SetText),
	    "x", sol::property(&DrawTextArgs::GetX, &DrawTextArgs::SetX),
	    "y", sol::property(&DrawTextArgs::GetY, &DrawTextArgs::SetY));

	TE_USERTYPE(
	    Engine,
	    "getVersion", &Engine::LuaGetVersion,
	    "mainWindow", GetMainWindowFromContext(context),
	    "quit", &Engine::Quit,
	    "triggerLoadPending", &Engine::TriggerLoadPending);

	TE_USERTYPE(
	    FontResources,
	    "getID", &FontResources::LuaGetID,
	    "getPath", &FontResources::LuaGetPath);

	TE_USERTYPE(
	    ImageResources,
	    "getID", &ImageResources::LuaGetID,
	    "getPath", &ImageResources::LuaGetPath);

	TE_USERTYPE(
	    Log,
	    "canOutput", &Log::CanOutput,
	    "output", &Log::LuaOutput);

	TE_USERTYPE(
	    RectangleF,
	    sol::call_constructor, sol::constructors<RectangleF()>(),
	    "x", sol::property(&RectangleF::LuaGet1, &RectangleF::LuaSet1),
	    "y", sol::property(&RectangleF::LuaGet2, &RectangleF::LuaSet2),
	    "w", sol::property(&RectangleF::LuaGet3, &RectangleF::LuaSet3),
	    "h", sol::property(&RectangleF::LuaGet4, &RectangleF::LuaSet4));

	TE_USERTYPE(
	    RenderBuffer,
	    "addRectangle", &RenderBuffer::LuaAddRectangle,
	    "clear", &RenderBuffer::Clear,
	    "draw", &RenderBuffer::LuaDraw);

	TE_USERTYPE(
	    RenderTarget,
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

	TE_USERTYPE(
	    Renderer,
	    "beginTarget", &Renderer::LuaBeginTarget,
	    "clear", &Renderer::LuaClear,
	    "drawRect", &Renderer::LuaDrawRect,
	    "drawText", &Renderer::LuaDrawText,
	    "endTarget", &Renderer::LuaEndTarget,
	    "newRenderBuffer", &Renderer::NewRenderBuffer,
	    "newRenderTarget", &Renderer::LuaNewRenderTarget);

	TE_USERTYPE(
	    Timer,
	    sol::call_constructor, sol::constructors<Timer(), Timer(bool paused)>(),
	    "getTime", &Timer::GetTime,
	    "pause", &Timer::Pause,
	    "reset", &Timer::Reset,
	    "unpause", &Timer::Unpause);

	TE_USERTYPE(
	    VirtualFileSystem,
	    "exists", &VirtualFileSystem::LuaExists,
	    "list", &VirtualFileSystem::LuaList,
	    "readFile", &VirtualFileSystem::LuaReadFile);

	TE_USERTYPE(
	    Window,
	    "renderer", &Window::renderer,
	    "close", &Window::Close,
	    "getHeight", &Window::GetHeight,
	    "getKey", &Window::LuaGetKey,
	    "getSize", &Window::GetSize,
	    "getWidth", &Window::GetWidth,
	    "shouldClose", &Window::ShouldClose);

	TE_CLASS(
	    OperatingSystem,
	    "getType", &OperatingSystem::GetType,
	    "isMobile", &OperatingSystem::IsMobile,
	    "isPC", &OperatingSystem::IsPC);

	TE_CLASS(
	    RandomDevice,
	    "entropy", &RandomDevice::Entropy,
	    "generate", &RandomDevice::LuaGenerate);

	TE_CLASS(
	    Time,
	    "getStartupTime", &Time::GetStartupTime,
	    "getSystemTime", &Time::GetSystemTime);

	lua["engine"] = &context.GetEngine();
	lua["localization"] = &dynamic_cast<Localization &>(context.GetLocalization());
	lua["vfs"] = &dynamic_cast<VirtualFileSystem &>(context.GetVirtualFileSystem());

	auto &collection = context.GetGlobalResourcesCollection();
	lua["binaries"] = &collection.GetBinariesResources();
	lua["fonts"] = &collection.GetFontResources();
	lua["images"] = &collection.GetImageResources();
	lua["texts"] = &collection.GetTextResources();
}

const std::vector<std::string_view> &LuaAPI::GetModGlobalsMigration() const noexcept
{
	if (_modGlobalsMigration.empty()) [[unlikely]]
	{
		_modGlobalsMigration = {
		    // luajit std
		    "_VERSION",
		    "assert",
		    "bit",
		    "coroutine",
		    "error",
		    "getmetatable",
		    "ipairs",
		    "math",
		    "newproxy",
		    "next",
		    "pairs",
		    "pcall",
		    "print",
		    "select",
		    "setmetatable",
		    "string",
		    "table",
		    "tonumber",
		    "tostring",
		    "type",
		    "unpack",
		    "utf8",
		    "xpcall",
		    // extensions
		    "string.buffer",
		    "table.clear",
		    "table.new",
		    // C++ enum classes
		    "EClientSessionState",
		    "EDebugConsoleCode",
		    "EDisconnectionCode",
		    "EEventInvocation",
		    "ELogVerbosity",
		    "EOperatingSystem",
		    "EPathListOption",
		    "EServerSessionState",
		    "EScanCode",
		    "ESocketType",
		    // C++ classes
		    "DrawRectArgs",
		    "DrawTextArgs",
		    "RectangleF",
		    "Timer",
		    // C++ static classes
		    "OperatingSystem",
		    "RandomDevice",
		    "Time",
		    // C++ exports
		    "binaries",
		    "engine",
		    "events",
		    "fonts",
		    "images",
		    "mods",
		    "network",
		    "scriptEngine",
		    "scriptErrors",
		    "scriptLoader",
		    "scriptProvider",
		    "vfs",
		};
	}
	return _modGlobalsMigration;
}
