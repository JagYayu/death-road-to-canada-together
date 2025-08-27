/**
 * @file mod/LuaAPI_Event.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "event/EventInvocation.hpp"
#include "event/EventManager.hpp"
#include "event/RuntimeEvent.hpp"
#include "mod/LuaAPI.hpp"

using namespace tudov;

#define TE_ENUM(Class, ...)     lua.new_enum<Class>(#Class, __VA_ARGS__)
#define TE_USERTYPE(Class, ...) lua.new_usertype<Class>(#Class, __VA_ARGS__)

void LuaAPI::InstallEvent(sol::state &lua, Context &context) noexcept
{
	TE_ENUM(EEventInvocation,
	        {
	            {"All", EEventInvocation::All},
	            {"None", EEventInvocation::None},
	            {"CacheHandlers", EEventInvocation::CacheHandlers},
	            {"NoProfiler", EEventInvocation::NoProfiler},
	            {"TrackProgression", EEventInvocation::TrackProgression},
	            {"Default", EEventInvocation::Default},
	        });

	TE_USERTYPE(RuntimeEvent,
	            "getInvokingScriptID", &RuntimeEvent::GetInvokingScriptID,
	            "getNextTrackID", &RuntimeEvent::GetNextTrackID);

	TE_USERTYPE(EventManager,
	            "add", &EventManager::LuaAdd,
	            "getInvokingEvent", &EventManager::GetInvokingEvent,
	            "new", &EventManager::LuaNew,
	            "invoke", &EventManager::LuaInvoke);

	lua["events"] = &dynamic_cast<EventManager &>(context.GetEventManager());
}
