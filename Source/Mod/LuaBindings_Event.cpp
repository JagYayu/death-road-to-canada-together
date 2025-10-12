/**
 * @file mod/LuaBindings_Event.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "mod/LuaBindings.hpp"

#include "Event/CoreEventsData.hpp"
#include "Event/EventInvocation.hpp"
#include "Event/EventManager.hpp"
#include "Event/RuntimeEvent.hpp"
#include "Program/Window.hpp"
#include "System/Keyboard.hpp"
#include "System/Mouse.hpp"
#include "Util/MicrosImpl.hpp"

#include <memory>

using namespace tudov;

void LuaBindings::InstallEvent(sol::state &lua, Context &context) noexcept
{
	TE_LB_ENUM(
	    EEventInvocation,
	    {
	        {"All", EEventInvocation::All},
	        {"None", EEventInvocation::None},
	        {"CacheHandlers", EEventInvocation::CacheHandlers},
	        {"NoProfiler", EEventInvocation::NoProfiler},
	        {"TrackProgression", EEventInvocation::TrackProgression},
	        {"Default", EEventInvocation::Default},
	    });

	TE_LB_USERTYPE(
	    RuntimeEvent,
	    "getInvokingScriptID", &RuntimeEvent::GetInvokingScriptID,
	    "getNextTrackID", &RuntimeEvent::GetNextTrackID);

	TE_LB_USERTYPE(
	    EventDebugProvideData,
	    "setDebugCommand", &EventDebugProvideData::LuaSetDebugCommand);

	TE_LB_USERTYPE(
	    EventKeyboardData,
	    "keyCode", &EventKeyboardData::keyCode,
	    "keyboard", &EventKeyboardData::keyboard,
	    "keyboardID", &EventKeyboardData::keyboardID,
	    "modifier", &EventKeyboardData::modifier,
	    "scanCode", &EventKeyboardData::scanCode,
	    "window", &EventKeyboardData::window,
	    "windowID", &EventKeyboardData::windowID);

	TE_LB_USERTYPE(
	    EventManager,
	    "add", &EventManager::LuaAdd,
	    "getInvokingEvent", &EventManager::GetInvokingEvent,
	    "new", &EventManager::LuaNew,
	    "invoke", &EventManager::LuaInvoke);

	TE_LB_USERTYPE(
	    EventMouseButtonData,
	    "button", &EventMouseButtonData::button,
	    "clicks", &EventMouseButtonData::clicks,
	    "window", &EventMouseButtonData::window,
	    "windowID", &EventMouseButtonData::windowID,
	    "mouse", &EventMouseButtonData::mouse,
	    "mouseID", &EventMouseButtonData::mouseID,
	    "x", &EventMouseButtonData::x,
	    "y", &EventMouseButtonData::y);

	TE_LB_USERTYPE(
	    EventMouseMotionData,
	    "window", &EventMouseMotionData::window,
	    "windowID", &EventMouseMotionData::windowID,
	    "mouse", &EventMouseMotionData::mouse,
	    "mouseID", &EventMouseMotionData::mouseID,
	    "relativeX", &EventMouseMotionData::relativeX,
	    "relativeY", &EventMouseMotionData::relativeY,
	    "x", &EventMouseMotionData::x,
	    "y", &EventMouseMotionData::y);

	TE_LB_USERTYPE(
	    EventScriptUnloadData,
	    "module", &EventScriptUnloadData::module,
	    "scriptID", &EventScriptUnloadData::scriptID,
	    "scriptName", &EventScriptUnloadData::scriptName);

	TE_LB_USERTYPE(
	    EventWindowResizeData,
	    "height", &EventWindowResizeData::height,
	    "width", &EventWindowResizeData::width,
	    "window", &EventWindowResizeData::window,
	    "windowID", &EventWindowResizeData::windowID);

	lua["TE"]["events"] = &dynamic_cast<EventManager &>(context.GetEventManager());
}
