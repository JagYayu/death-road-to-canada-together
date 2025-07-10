#pragma once

#include "CoreEvents.hpp"
#include "DelegateEvent.hpp"
#include "LoadtimeEvent.hpp"
#include "RuntimeEvent.hpp"
#include "program/EngineComponent.hpp"
#include "util/Defs.hpp"

#include <memory>
#include <sol/sol.hpp>

#include <string_view>

namespace tudov
{
	class Context;
	class Engine;
	class IModManager;
	class IScriptLoader;

	struct IEventManager : public IEngineComponent
	{
		virtual ICoreEvents &GetCoreEvents() noexcept = 0;
		[[nodiscard]] virtual ScriptID GetEventIDByName(std::string_view scriptName) const noexcept = 0;
		[[nodiscard]] virtual std::optional<std::string_view> GetEventNameByID(EventID eventID) const noexcept = 0;
		[[nodiscard]] virtual bool IsValidEventID(EventID eventID) const noexcept = 0;
		virtual void ClearInvalidScriptEvents() noexcept = 0;

		[[nodiscard]] virtual std::unordered_map<EventID, std::shared_ptr<RuntimeEvent>>::const_iterator BeginRuntimeEvents() const = 0;
		[[nodiscard]] virtual std::unordered_map<EventID, std::shared_ptr<RuntimeEvent>>::const_iterator EndRuntimeEvents() const = 0;

		inline const ICoreEvents &GetCoreEvents() const noexcept
		{
			return const_cast<IEventManager *>(this)->GetCoreEvents();
		}
	};

	class EventManager : public IEventManager
	{
		friend class CoreEvents;
		friend class LuaAPI;

	  protected:
		Context &_context;
		std::shared_ptr<Log> _log;
		std::unique_ptr<CoreEvents> _coreEvents;

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
		explicit EventManager(Context &context) noexcept;
		~EventManager() noexcept override;

	  private:
		[[nodiscard]] EventID AllocEventID(std::string_view eventName) noexcept;
		void DeallocEventID(EventID eventID) noexcept;
		void OnScriptsLoaded();
		[[nodiscard]] std::optional<std::reference_wrapper<AbstractEvent>> TryGetRegistryEvent(EventID eventID);

		void LuaAdd(const sol::object &event, const sol::object &func, const sol::object &name, const sol::object &order, const sol::object &key, const sol::object &sequence);
		EventID LuaNew(const sol::object &event, const sol::object &orders, const sol::object &keys);
		void LuaInvoke(const sol::object &event, const sol::object &args, const sol::object &key, const sol::object &uncached);

	  public:
		Context &GetContext() noexcept override;
		void Initialize() noexcept override;
		void Deinitialize() noexcept override;
		ICoreEvents &GetCoreEvents() noexcept override;
		void InstallToScriptEngine(IScriptEngine &scriptEngine);
		void UninstallFromScriptEngine(IScriptEngine &scriptEngine);
		void AttachToScriptLoader(IScriptLoader &scriptLoader) noexcept;
		void DetachFromScriptLoader(IScriptLoader &scriptLoader) noexcept;

		[[nodiscard]] ScriptID GetEventIDByName(std::string_view scriptName) const noexcept override;
		[[nodiscard]] std::optional<std::string_view> GetEventNameByID(EventID eventID) const noexcept override;
		[[nodiscard]] bool IsValidEventID(EventID eventID) const noexcept override;
		void ClearInvalidScriptEvents() noexcept override;

		[[nodiscard]] std::unordered_map<EventID, std::shared_ptr<RuntimeEvent>>::const_iterator BeginRuntimeEvents() const override;
		[[nodiscard]] std::unordered_map<EventID, std::shared_ptr<RuntimeEvent>>::const_iterator EndRuntimeEvents() const override;
	};
} // namespace tudov
