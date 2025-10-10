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

#include "event/CoreEventsData.hpp"
#include "event/EventInvocation.hpp"
#include "event/EventManager.hpp"
#include "event/RuntimeEvent.hpp"
#include "util/MicrosImpl.hpp"
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
	    EventManager,
	    "add", &EventManager::LuaAdd,
	    "getInvokingEvent", &EventManager::GetInvokingEvent,
	    "new", &EventManager::LuaNew,
	    "invoke", &EventManager::LuaInvoke);

		std::weak_ptr<int> a;

	TE_LB_USERTYPE(
	    EventMouseMotionData,
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

	lua["TE"]["events"] = &dynamic_cast<EventManager &>(context.GetEventManager());
}
