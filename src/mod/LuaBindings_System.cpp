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

#include "program/Window.hpp"
#include "system/Keyboard.hpp"
#include "system/KeyboardManager.hpp"
#include "system/Mouse.hpp"
#include "system/MouseButton.hpp"
#include "system/MouseManager.hpp"
#include "system/OperatingSystem.hpp"
#include "system/RandomDevice.hpp"
#include "system/Time.hpp"
#include "util/MicrosImpl.hpp"

using namespace tudov;

void LuaBindings::InstallSystem(sol::state &lua, Context &context) noexcept
{
	TE_LB_ENUM(
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

	TE_LB_ENUM(
	    EMouseButton,
	    {
	        {"None", EMouseButton::None},
	        {"Left", EMouseButton::Left},
	        {"Middle", EMouseButton::Middle},
	        {"Right", EMouseButton::Right},
	        {"Extra1", EMouseButton::Extra1},
	        {"Extra2", EMouseButton::Extra2},
	    });

	TE_LB_ENUM(
	    EMouseButtonFlag,
	    {
	        {"Left", EMouseButtonFlag::Left},
	        {"Middle", EMouseButtonFlag::Middle},
	        {"Right", EMouseButtonFlag::Right},
	        {"Extra1", EMouseButtonFlag::Extra1},
	        {"Extra2", EMouseButtonFlag::Extra2},
	    });

	TE_LB_ENUM(
	    EOperatingSystem,
	    {
	        {"Unknown", EOperatingSystem::Unknown},
	        {"Windows", EOperatingSystem::Windows},
	        {"Linux", EOperatingSystem::Linux},
	        {"MaxOS", EOperatingSystem::MaxOS},
	        {"Android", EOperatingSystem::Android},
	        {"IOS", EOperatingSystem::IOS},
	    });

	TE_LB_USERTYPE(
	    Timer,
	    sol::call_constructor, sol::constructors<Timer(), Timer(bool paused)>(),
	    "getTime", &Timer::GetTime,
	    "pause", &Timer::Pause,
	    "reset", &Timer::Reset,
	    "unpause", &Timer::Unpause);

	TE_LB_USERTYPE(
	    Keyboard,
	    "getKeyboardID", &Keyboard::GetKeyboardID,
	    "isKeyCodeHeld", &Keyboard::LuaIsKeyCodeHeld,
	    "isScanCodeHeld", &Keyboard::LuaIsScanCodeHeld,
	    "listHeldKeyCodes", &Keyboard::LuaListHeldKeyCodes,
	    "listHeldScanCodes", &Keyboard::LuaListHeldScanCodes);

	TE_LB_USERTYPE(
	    KeyboardManager,
	    "getKeyboardAt", &KeyboardManager::LuaGetKeyboardAt,
	    "getKeyboardByID", &KeyboardManager::LuaGetKeyboardByID,
	    "listKeyboards", &KeyboardManager::LuaListKeyboards);

	TE_LB_USERTYPE(
	    Mouse,
	    "getFocusedWindow", &Mouse::LuaGetFocusedWindow,
	    "getID", &Mouse::GetMouseID,
	    "getName", &Mouse::GetMouseName,
	    "getPosition", &Mouse::GetMousePosition,
	    "getState", &Mouse::GetMouseState,
	    "isButtonDown", &Mouse::IsMouseButtonDown,
	    "isButtonsDown", &Mouse::IsMouseButtonsDown);

	TE_LB_USERTYPE(
	    MouseManager,
	    "getMouseAt", &MouseManager::GetMouseAt,
	    "getMouseByID", &MouseManager::GetMouseByID,
	    "getPrimaryMouse", &MouseManager::LuaGetPrimaryMouse);

	TE_LB_CLASS(
	    OperatingSystem,
	    "getType", &OperatingSystem::GetType,
	    "isMobile", &OperatingSystem::IsMobile,
	    "isPC", &OperatingSystem::IsPC);

	TE_LB_CLASS(
	    RandomDevice,
	    "entropy", &RandomDevice::Entropy,
	    "generate", &RandomDevice::LuaGenerate);

	TE_LB_CLASS(
	    Time,
	    "getStartupTime", &Time::GetStartupTime,
	    "getSystemTime", &Time::GetSystemTime);

	lua["mouses"] = &dynamic_cast<MouseManager &>(context.GetMouseManager());
	lua["keyboards"] = &dynamic_cast<KeyboardManager &>(context.GetKeyboardManager());
}
