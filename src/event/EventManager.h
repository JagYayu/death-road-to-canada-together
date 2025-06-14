#pragma once

#include "DelegateEvent.hpp"
#include "LoadtimeEvent.h"
#include "RuntimeEvent.h"
#include "event/AddHandlerArgs.hpp"
#include "util/Defs.h"
#include "util/StringUtils.hpp"

#include <sol/sol.hpp>

#include <string_view>

namespace tudov
{
	class ModManager;

	class EventManager
	{
	  private:
		SharedPtr<Log> _log;
		EventID _latestEventID;
		UnorderedMap<StringView, EventID> _eventName2ID;
		UnorderedMap<EventID, String> _eventID2Name;
		UnorderedMap<EventID, SharedPtr<LoadtimeEvent>> _loadtimeEvents;
		UnorderedMap<EventID, SharedPtr<RuntimeEvent>> _runtimeEvents;
		Vector<SharedPtr<RuntimeEvent>> _staticEvents;

		DelegateEvent<>::HandlerID _onPreLoadAllScriptsHandlerID;
		DelegateEvent<>::HandlerID _onPostLoadAllScriptsHandlerID;
		DelegateEvent<ScriptID>::HandlerID _onPreHotReloadScriptsHandlerID;
		DelegateEvent<ScriptID>::HandlerID _onPostHotReloadScriptsHandlerID;

	  public:
		ModManager &modManager;
		SharedPtr<RuntimeEvent> update;
		SharedPtr<RuntimeEvent> render;

	  public:
		explicit EventManager(ModManager &modManager);
		~EventManager();

	  private:
		[[nodiscard]]
		EventID AllocEventID(StringView eventName) noexcept;
		void DeallocEventID(EventID eventID) noexcept;

		void OnScriptsLoaded();

	  public:
		void RegisterGlobal(ScriptEngine &scriptEngine);
		void UnregisterGlobal(ScriptEngine &scriptEngine);

		[[nodiscard]]
		ScriptID GetEventIDByName(StringView scriptName) const noexcept;
		[[nodiscard]]
		Optional<StringView> GetEventNameByID(EventID eventID) const noexcept;
		[[nodiscard]]
		bool IsValidEventID(EventID eventID) const noexcept;

		Optional<Reference<AbstractEvent>> TryGetRegistryEvent(EventID eventID);

		UnorderedMap<EventID, SharedPtr<RuntimeEvent>>::const_iterator BeginRuntimeEvents() const;
		UnorderedMap<EventID, SharedPtr<RuntimeEvent>>::const_iterator EndRuntimeEvents() const;
	};
} // namespace tudov
