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
		std::unordered_map<std::string_view, EventID> _eventName2ID;
		std::unordered_map<EventID, std::string> _eventID2Name;
		std::unordered_map<EventID, SharedPtr<LoadtimeEvent>> _loadtimeEvents;
		std::unordered_map<EventID, SharedPtr<RuntimeEvent>> _runtimeEvents;
		std::vector<SharedPtr<RuntimeEvent>> _staticEvents;

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
		EventID AllocEventID(std::string_view eventName) noexcept;
		void DeallocEventID(EventID eventID) noexcept;

		void OnScriptsLoaded();

	  public:
		void RegisterGlobal(ScriptEngine &scriptEngine);
		void UnregisterGlobal(ScriptEngine &scriptEngine);

		[[nodiscard]]
		ScriptID GetEventIDByName(std::string_view scriptName) const noexcept;
		[[nodiscard]]
		std::optional<std::string_view> GetEventNameByID(EventID eventID) const noexcept;
		[[nodiscard]]
		bool IsValidEventID(EventID eventID) const noexcept;

		std::optional<Reference<AbstractEvent>> TryGetRegistryEvent(EventID eventID);

		std::unordered_map<EventID, SharedPtr<RuntimeEvent>>::const_iterator BeginRuntimeEvents() const;
		std::unordered_map<EventID, SharedPtr<RuntimeEvent>>::const_iterator EndRuntimeEvents() const;
	};
} // namespace tudov
