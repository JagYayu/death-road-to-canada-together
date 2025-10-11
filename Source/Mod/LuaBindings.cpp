/**
 * @file mod/LuaBindings.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "mod/LuaBindings.hpp"

#include "Data/VirtualFileSystem.hpp"
#include "Debug/Debug.hpp"
#include "Graphic/Camera2D.hpp"
#include "Graphic/RenderArgs.hpp"
#include "Graphic/RenderBuffer.hpp"
#include "Graphic/RenderTarget.hpp"
#include "Graphic/Renderer.hpp"
#include "I18n/Localization.hpp"
#include "Math/Geometry.hpp"
#include "Program/Engine.hpp"
#include "Program/PrimaryWindow.hpp"
#include "Program/Window.hpp"
#include "Program/WindowManager.hpp"
#include "Resource/FontResources.hpp"
#include "Resource/GlobalResourcesCollection.hpp"
#include "Resource/ImageResources.hpp"
#include "Util/MicrosImpl.hpp"

#include "sol/forward.hpp"
#include "sol/property.hpp"

#include <memory>

using namespace tudov;
using namespace tudov::impl;

static std::vector<std::string_view> emptyModGlobalsMigration = {};

const std::vector<std::string_view> &ILuaBindings::GetModGlobalsMigration() const noexcept
{
	return emptyModGlobalsMigration;
}

bool LuaBindings::TypeID::LuaEqualTo(const TypeID &typeID) const noexcept
{
	if (this->typeinfo == nullptr || typeID.typeinfo == nullptr)
	{
		return false;
	}
	return *this->typeinfo == *typeID.typeinfo;
}

// bool LuaBindings::RegisterInstallation(std::string_view name, const TInstallation &installation)
// {
// 	for (auto &&installation : _installations)
// 	{
// 		if (std::get<0>(installation) == name) [[unlikely]]
// 		{
// 			return false;
// 		}
// 	}

// 	_installations.emplace_back(name, installation);
// 	return true;
// }

decltype(auto) GetPrimaryWindowFromContext(Context &context) noexcept
{
	return sol::readonly_property([&context]() -> std::shared_ptr<Window>
	{
		try
		{
			auto window = context.GetWindowManager().GetPrimaryWindow();
			return std::static_pointer_cast<Window>(window);
		}
		catch (std::exception &e)
		{
			return nullptr;
		}
	});
}

#define TE_USERDATA_LUA_ARRAY(Type) TE_LB_USERTYPE( \
	LuaArray##Type,                                 \
	"clear", &LuaArray##Type::Clear,                \
	"get", &LuaArray##Type::Get,                    \
	"getCapacity", &LuaArray##Type::GetCapacity,    \
	"getCount", &LuaArray##Type::GetCount,          \
	"set", &LuaArray##Type::Set);

void LuaBindings::ProvideLuaBindings(sol::state &lua, Context &context)
{
	TE_LB_ENUM(
	    EDebugConsoleCode,
	    {
	        {"None", EDebugConsoleCode::None},
	        {"Failure", EDebugConsoleCode::Failure},
	        {"Success", EDebugConsoleCode::Success},
	        {"Warn", EDebugConsoleCode::Warn},
	    });

	TE_LB_ENUM(
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

	TE_USERDATA_LUA_ARRAY(Bit);
	TE_USERDATA_LUA_ARRAY(F32);
	TE_USERDATA_LUA_ARRAY(F64);
	TE_USERDATA_LUA_ARRAY(I8);
	TE_USERDATA_LUA_ARRAY(I16);
	TE_USERDATA_LUA_ARRAY(I32);
	TE_USERDATA_LUA_ARRAY(I64);
	TE_USERDATA_LUA_ARRAY(U8);
	TE_USERDATA_LUA_ARRAY(U16);
	TE_USERDATA_LUA_ARRAY(U32);
	TE_USERDATA_LUA_ARRAY(U64);

	TE_LB_USERTYPE(
	    DrawRectArgs,
	    sol::call_constructor, sol::constructors<DrawRectArgs()>(),
	    "angle", sol::property(&DrawRectArgs::LuaGetAngle, &DrawRectArgs::LuaSetAngle),
	    "color", sol::property(&DrawRectArgs::LuaGetColor, &DrawRectArgs::LuaSetColor),
	    "destination", sol::property(&DrawRectArgs::LuaGetDestination, &DrawRectArgs::LuaSetDestination),
	    // "origin", sol::property(&DrawRectArgs::LuaGetOrigin, &DrawRectArgs::LuaSetOrigin),
	    "source", sol::property(&DrawRectArgs::LuaGetSource, &DrawRectArgs::LuaSetSource),
	    "texture", sol::property(&DrawRectArgs::LuaGetTexture, &DrawRectArgs::LuaSetTexture));

	TE_LB_USERTYPE(
	    DrawTextArgs,
	    sol::call_constructor, sol::constructors<DrawTextArgs()>(),
	    "alignX", sol::property(&DrawTextArgs::GetAlignX, &DrawTextArgs::SetAlignX),
	    "alignY", sol::property(&DrawTextArgs::GetAlignY, &DrawTextArgs::SetAlignY),
	    "backgroundColor", sol::property(&DrawTextArgs::GetBackgroundColor, &DrawTextArgs::SetBackgroundColor),
	    "characterSize", sol::property(&DrawTextArgs::GetCharacterSize, &DrawTextArgs::SetCharacterSize),
	    "color", sol::property(&DrawTextArgs::GetColor, &DrawTextArgs::SetColor),
	    "font", sol::property(&DrawTextArgs::GetFont, &DrawTextArgs::SetFont),
	    "maxWidth", sol::property(&DrawTextArgs::GetMaxWidth, &DrawTextArgs::SetMaxWidth),
	    "scale", sol::property(&DrawTextArgs::GetScale, &DrawTextArgs::SetScale),
	    "shadow", sol::property(&DrawTextArgs::GetShadow, &DrawTextArgs::SetShadow),
	    "shadowColor", sol::property(&DrawTextArgs::GetShadowColor, &DrawTextArgs::SetShadowColor),
	    "text", sol::property(&DrawTextArgs::GetText, &DrawTextArgs::SetText),
	    "x", sol::property(&DrawTextArgs::GetX, &DrawTextArgs::SetX),
	    "y", sol::property(&DrawTextArgs::GetY, &DrawTextArgs::SetY));

	TE_LB_USERTYPE(
	    Engine,
	    "getBeginTick", &Engine::GetBeginTick,
	    "getTick", &Engine::GetTick,
	    "getLoadingBeginTick", &Engine::GetLoadingBeginTick,
	    "getVersion", &Engine::LuaGetVersion,
	    "primaryWindow", GetPrimaryWindowFromContext(context),
	    "quit", &Engine::Quit,
	    "triggerLoadPending", &Engine::TriggerLoadPending);

	TE_LB_USERTYPE(
	    FontResources,
	    "getID", &FontResources::LuaGetID,
	    "getPath", &FontResources::LuaGetPath);

	TE_LB_USERTYPE(
	    ImageResources,
	    "getID", &ImageResources::LuaGetID,
	    "getPath", &ImageResources::LuaGetPath);

	TE_LB_USERTYPE(
	    Log,
	    "canOutput", &Log::CanOutput,
	    "output", &Log::LuaOutput);

	TE_LB_USERTYPE(
	    RectangleF,
	    sol::call_constructor, sol::constructors<RectangleF()>(),
	    "x", sol::property(&RectangleF::LuaGetX, &RectangleF::LuaSetX),
	    "y", sol::property(&RectangleF::LuaGetY, &RectangleF::LuaSetY),
	    "w", sol::property(&RectangleF::LuaGetW, &RectangleF::LuaSetW),
	    "h", sol::property(&RectangleF::LuaGetH, &RectangleF::LuaSetH));

	TE_LB_USERTYPE(
	    RenderBuffer,
	    "addRectangle", &RenderBuffer::LuaAddRectangle,
	    "clear", &RenderBuffer::Clear,
	    "draw", &RenderBuffer::LuaDraw);

	TE_LB_USERTYPE(
	    RenderTarget,
	    // "getBlendMode", &RenderTarget::GetBlendMode,
	    "getCameraTargetScale", &RenderTarget::GetCameraTargetScale,
	    "getHeight", &RenderTarget::GetHeight,
	    "getPosition", &RenderTarget::GetCameraPosition,
	    "getPositionLerpFactor", &RenderTarget::GetPositionLerpFactor,
	    "getScale", &RenderTarget::GetCameraScale,
	    "getScaleLerpFactor", &RenderTarget::GetScaleLerpFactor,
	    "getSize", &RenderTarget::GetSize,
	    "getWidth", &RenderTarget::GetWidth,
	    "resize", &RenderTarget::Resize,
	    "resizeToFit", &RenderTarget::ResizeToFit,
	    // "setBlendMode", &RenderTarget::SetBlendMode,
	    "setCameraTargetPosition", &RenderTarget::SetCameraTargetPosition,
	    "setCameraTargetScale", &RenderTarget::SetCameraTargetScale,
	    "setPositionLerpFactor", &RenderTarget::SetPositionLerpFactor,
	    "setScaleLerpFactor", &RenderTarget::SetScaleLerpFactor,
	    "snapCameraPosition", &RenderTarget::SnapCameraPosition,
	    "snapCameraScale", &RenderTarget::SnapCameraScale,
	    "update", &RenderTarget::Update);

	TE_LB_USERTYPE(
	    Renderer,
	    "beginTarget", &Renderer::LuaBeginTarget,
	    "clear", &Renderer::LuaClear,
	    "drawRect", &Renderer::LuaDrawRect,
	    "drawText", &Renderer::LuaDrawText,
	    "endTarget", &Renderer::LuaEndTarget,
	    "newRenderBuffer", &Renderer::NewRenderBuffer,
	    "newRenderTarget", &Renderer::LuaNewRenderTarget);

	TE_LB_USERTYPE(
	    TypeID,
	    sol::meta_function::equal_to, &TypeID::LuaEqualTo);

	TE_LB_USERTYPE(
	    VirtualFileSystem,
	    "exists", &VirtualFileSystem::LuaExists,
	    "list", &VirtualFileSystem::LuaList,
	    "readFile", &VirtualFileSystem::LuaReadFile);

	TE_LB_USERTYPE(
	    Window,
	    "close", &Window::Close,
	    "getHeight", &Window::GetHeight,
	    "getKey", &Window::LuaGetKey,
	    "getSize", &Window::GetSize,
	    "getWidth", &Window::GetWidth,
	    "getWindowID", &Window::GetWindowID,
	    "renderer", &Window::_renderer,
	    "shouldClose", &Window::ShouldClose);

	auto &collection = context.GetGlobalResourcesCollection();

	TE_LB_CLASS(
	    TE,
	    "binaries", &collection.GetBinariesResources(),
	    "engine", &context.GetEngine(),
	    "fonts", &collection.GetFontResources(),
	    "images", &collection.GetImageResources(),
	    "i18n", &dynamic_cast<Localization &>(context.GetLocalization()),
	    "texts", &collection.GetTextResources(),
	    "vfs", &dynamic_cast<VirtualFileSystem &>(context.GetVirtualFileSystem()));

	InstallEvent(lua, context);
	InstallKeyModifier(lua, context);
	InstallMod(lua, context);
	InstallNetwork(lua, context);
	InstallScanCode(lua, context);
	InstallSystem(lua, context);
}

const std::vector<std::string_view> &LuaBindings::GetModGlobalsMigration() const noexcept
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
		    "EMouseButton",
		    "EMouseButtonFlag",
		    "EOperatingSystem",
		    "EPathListOption",
		    "EServerSessionState",
		    "EScanCode",
		    "ESocketType",
		    // C++ classes
		    "DrawRectArgs",
		    "DrawTextArgs",
		    "RectangleF",
		    "TE",
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
		    "keyboards",
		    "mice",
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
