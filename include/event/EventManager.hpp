/**
 * @file event/EventManager.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "CoreEvents.hpp"
#include "LoadtimeEvent.hpp"
#include "RuntimeEvent.hpp"
#include "mod/LuaAPI.hpp"
#include "program/EngineComponent.hpp"
#include "sol/forward.hpp"
#include "util/Definitions.hpp"

#include <memory>
#include <optional>

namespace tudov
{
	class Context;
	class Engine;

	struct IEventManager : public IEngineComponent
	{
		~IEventManager() noexcept override = default;

		virtual ICoreEvents &GetCoreEvents() noexcept = 0;
		[[nodiscard]] virtual ScriptID GetEventIDByName(std::string_view scriptName) const noexcept = 0;
		[[nodiscard]] virtual std::optional<std::string_view> GetEventNameByID(EventID eventID) const noexcept = 0;
		[[nodiscard]] virtual bool IsValidEventID(EventID eventID) const noexcept = 0;
		virtual void ClearInvalidScriptEvents() noexcept = 0;

		[[nodiscard]] virtual std::size_t GetRuntimeEventsCount() const noexcept = 0;
		[[nodiscard]] virtual std::unordered_map<EventID, std::shared_ptr<RuntimeEvent>>::const_iterator BeginRuntimeEvents() const = 0;
		[[nodiscard]] virtual std::unordered_map<EventID, std::shared_ptr<RuntimeEvent>>::const_iterator EndRuntimeEvents() const = 0;

		inline const ICoreEvents &GetCoreEvents() const noexcept
		{
			return const_cast<IEventManager *>(this)->GetCoreEvents();
		}
	};

	class EventManager : public IEventManager, private ILogProvider
	{
		friend class CoreEvents;
		friend class LuaAPI;

	  private:
		static ILuaAPI::TInstallation rendererLuaAPIInstallation;

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

		DelegateEventHandlerID _onPreLoadAllScriptsHandlerID = 0;
		DelegateEventHandlerID _onPostLoadAllScriptsHandlerID = 0;
		DelegateEventHandlerID _onPreHotReloadScriptsHandlerID = 0;
		DelegateEventHandlerID _onPostHotReloadScriptsHandlerID = 0;
		DelegateEventHandlerID _onUnloadScriptHandlerID = 0;

	  public:
		explicit EventManager(Context &context) noexcept;
		~EventManager() noexcept override;

	  public:
		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;
		void PreInitialize() noexcept override;
		void Initialize() noexcept override;
		void Deinitialize() noexcept override;
		void PostDeinitialize() noexcept override;

		ICoreEvents &GetCoreEvents() noexcept override;
		void InstallToScriptEngine(IScriptEngine &scriptEngine);
		void UninstallFromScriptEngine(IScriptEngine &scriptEngine);

		[[nodiscard]] ScriptID GetEventIDByName(std::string_view scriptName) const noexcept override;
		[[nodiscard]] std::optional<std::string_view> GetEventNameByID(EventID eventID) const noexcept override;
		[[nodiscard]] bool IsValidEventID(EventID eventID) const noexcept override;
		void ClearInvalidScriptEvents() noexcept override;

		[[nodiscard]] std::size_t GetRuntimeEventsCount() const noexcept override;
		[[nodiscard]] std::unordered_map<EventID, std::shared_ptr<RuntimeEvent>>::const_iterator BeginRuntimeEvents() const override;
		[[nodiscard]] std::unordered_map<EventID, std::shared_ptr<RuntimeEvent>>::const_iterator EndRuntimeEvents() const override;

	  private:
		[[nodiscard]] EventID AllocEventID(std::string_view eventName) noexcept;
		void DeallocEventID(EventID eventID) noexcept;
		void OnScriptsLoaded();
		void EmplaceRuntimeEventFromLoadtimeEvent(const std::shared_ptr<LoadtimeEvent> &loadtimeEvent);
		[[nodiscard]] std::optional<std::reference_wrapper<AbstractEvent>> TryGetRegistryEvent(EventID eventID);

		void LuaAdd(sol::object event, sol::object func, sol::object name, sol::object order, sol::object key, sol::object sequence);
		EventID LuaNew(sol::object event, sol::object orders, sol::object keys);
		void LuaInvoke(sol::object event, sol::object args, sol::object key, sol::object options);

		std::optional<ScriptID> TryBuildEvent(EventID eventID, ScriptID scriptID, std::vector<std::string> orders, std::vector<EventHandleKey> keys) noexcept;
	};
} // namespace tudov
