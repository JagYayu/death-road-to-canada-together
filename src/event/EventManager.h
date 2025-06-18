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
	class ScriptLoader;

	class EventManager
	{
	  private:
		std::shared_ptr<Log> _log;
		EventID _latestEventID;
		std::unordered_map<std::string_view, EventID> _eventName2ID;
		std::unordered_map<EventID, std::string> _eventID2Name;
		std::unordered_map<EventID, std::shared_ptr<LoadtimeEvent>> _loadtimeEvents;
		std::unordered_map<EventID, std::shared_ptr<RuntimeEvent>> _runtimeEvents;
		std::vector<std::shared_ptr<RuntimeEvent>> _staticEvents;

		DelegateEvent<>::HandlerID _onPreLoadAllScriptsHandlerID;
		DelegateEvent<>::HandlerID _onPostLoadAllScriptsHandlerID;
		DelegateEvent<ScriptID>::HandlerID _onPreHotReloadScriptsHandlerID;
		DelegateEvent<ScriptID>::HandlerID _onPostHotReloadScriptsHandlerID;
		DelegateEvent<ScriptID>::HandlerID _onUnloadScriptHandlerID;

	  public:
		ModManager &modManager;
		std::shared_ptr<RuntimeEvent> update;
		std::shared_ptr<RuntimeEvent> render;
		std::shared_ptr<RuntimeEvent> keyDown;
		std::shared_ptr<RuntimeEvent> keyUp;
		std::shared_ptr<RuntimeEvent> mouseMove;
		std::shared_ptr<RuntimeEvent> mouseButtonDown;
		std::shared_ptr<RuntimeEvent> mouseButtonUp;
		std::shared_ptr<RuntimeEvent> mouseWheel;

	  public:
		explicit EventManager(ModManager &modManager);
		~EventManager();

	  private:
		[[nodiscard]]
		EventID AllocEventID(std::string_view eventName) noexcept;
		void DeallocEventID(EventID eventID) noexcept;
		void OnScriptsLoaded();

	  public:
		void InstallToScriptEngine(ScriptEngine &scriptEngine);
		void UninstallFromScriptEngine(ScriptEngine &scriptEngine);
		void AttachToScriptLoader(ScriptLoader &scriptLoader) noexcept;
		void DetachFromScriptLoader(ScriptLoader &scriptLoader) noexcept;

		[[nodiscard]]
		ScriptID GetEventIDByName(std::string_view scriptName) const noexcept;
		[[nodiscard]]
		std::optional<std::string_view> GetEventNameByID(EventID eventID) const noexcept;
		[[nodiscard]]
		bool IsValidEventID(EventID eventID) const noexcept;
		void ClearInvalidScriptEvents() noexcept;
		[[nodiscard]]
		std::optional<std::reference_wrapper<AbstractEvent>> TryGetRegistryEvent(EventID eventID);

		[[nodiscard]]
		std::unordered_map<EventID, std::shared_ptr<RuntimeEvent>>::const_iterator BeginRuntimeEvents() const;
		[[nodiscard]]
		std::unordered_map<EventID, std::shared_ptr<RuntimeEvent>>::const_iterator EndRuntimeEvents() const;
	};
} // namespace tudov
