/**
 * @file mod/LuaBindings_System.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "mod/LuaBindings.hpp"

#include "system/Keyboard.hpp"
#include "system/KeyboardManager.hpp"
#include "system/Mouse.hpp"
#include "system/MouseManager.hpp"
#include "system/OperatingSystem.hpp"
#include "system/RandomDevice.hpp"
#include "system/Time.hpp"

using namespace tudov;

#define TE_ENUM(Class, ...)     lua.new_enum<Class>(#Class, __VA_ARGS__)
#define TE_USERTYPE(Class, ...) lua.new_usertype<Class>(#Class, __VA_ARGS__)
#define TE_CLASS(Class, ...)    lua.create_named_table(("" #Class ""), __VA_ARGS__)

void LuaBindings::InstallSystem(sol::state &lua, Context &context) noexcept
{
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

	TE_USERTYPE(
	    Timer,
	    sol::call_constructor, sol::constructors<Timer(), Timer(bool paused)>(),
	    "getTime", &Timer::GetTime,
	    "pause", &Timer::Pause,
	    "reset", &Timer::Reset,
	    "unpause", &Timer::Unpause);

	TE_USERTYPE(
	    Keyboard,
	    "getKeyboardID", &Keyboard::GetKeyboardID,
	    "isKeyCodeHeld", &Keyboard::LuaIsKeyCodeHeld,
	    "isScanCodeHeld", &Keyboard::LuaIsScanCodeHeld,
	    "listHeldKeyCodes", &Keyboard::LuaListHeldKeyCodes,
	    "listHeldScanCodes", &Keyboard::LuaListHeldScanCodes);

	TE_USERTYPE(
	    KeyboardManager,
	    "getKeyboardAt", &KeyboardManager::LuaGetKeyboardAt,
	    "getKeyboardByID", &KeyboardManager::LuaGetKeyboardByID);

	TE_USERTYPE(
	    Mouse,
	    "getMouseID", &Mouse::GetMouseID);

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

	lua["mouses"] = &dynamic_cast<MouseManager &>(context.GetMouseManager());
	lua["keyboards"] = &dynamic_cast<KeyboardManager &>(context.GetKeyboardManager());
}
