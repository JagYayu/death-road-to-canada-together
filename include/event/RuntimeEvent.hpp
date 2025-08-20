/**
 * @file event/RuntimeEvent.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "AbstractEvent.hpp"
#include "EventHandler.hpp"
#include "EventInvocation.hpp"
#include "debug/EventProfiler.hpp"
#include "util/Definitions.hpp"
#include "util/Log.hpp"

#include "sol/forward.hpp"
#include "sol/types.hpp"

#include <cstdint>
#include <optional>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

namespace tudov
{
	struct CoreEventData;
	struct IEventProfiler;
	struct IScriptEngine;
	struct IScriptProvider;

	class RuntimeEvent : public AbstractEvent, private ILogProvider
	{
		using InvocationCache = std::vector<EventHandler *>;
		using ProgressionID = std::uint32_t;

	  private:
		struct Progression
		{
			std::size_t value;
			std::size_t total;
		};

		struct Profile
		{
			bool traceHandlers;
			EventProfiler eventProfiler;

			template <typename... TArgs>
			explicit Profile(bool traceHandlers, TArgs &&...args) noexcept
			    : traceHandlers(traceHandlers),
			      eventProfiler(std::move<TArgs>(args)...)
			{
			}
		};

	  private:
		std::shared_ptr<Log> _log;
		std::shared_ptr<Profile> _profile;
		bool _handlersSortedCache;
		std::optional<InvocationCache> _invocationCache;
		std::unordered_map<EventHandleKey, InvocationCache, EventHandleKey::Hash, EventHandleKey::Equal> _invocationCaches;
		std::vector<std::string> _orders;
		std::unordered_set<EventHandleKey, EventHandleKey::Hash, EventHandleKey::Equal> _keys;
		std::vector<EventHandler> _handlers;
		ProgressionID _invocationTrackID;
		std::unordered_map<ProgressionID, Progression> _invocationTracks;

	  public:
		explicit RuntimeEvent(IEventManager &eventManager, EventID eventID, const std::vector<std::string> &orders = {""}, const std::unordered_set<EventHandleKey, EventHandleKey::Hash, EventHandleKey::Equal> &keys = {}, ScriptID scriptID = false);
		~RuntimeEvent() noexcept override;

	  public:
		Log &GetLog() noexcept override;

		std::vector<EventHandler>::const_iterator BeginHandlers() const noexcept;
		std::vector<EventHandler>::const_iterator EndHandlers() const noexcept;

		[[nodiscard]] RuntimeEvent::Profile *GetProfile() const noexcept;
		void EnableProfiler(bool traceHandlers) noexcept;
		void DisableProfiler() noexcept;

		ProgressionID GetNextTrackID() noexcept;
		std::tuple<std::size_t, std::size_t> GetProgression() noexcept;

		void Add(const AddHandlerArgs &args) override;

		void Invoke(const sol::object &e = sol::lua_nil, const EventHandleKey &key = {}, EEventInvocation options = EEventInvocation::Default);
		[[deprecated]] void InvokeUncached(const sol::object &e = sol::lua_nil, const EventHandleKey &key = {});

		void Invoke(IScriptEngine &scriptEngine, CoreEventData *data, const EventHandleKey &key = {}, EEventInvocation options = EEventInvocation::Default);

		ProgressionID GetNextInvocationID() noexcept;

		void ClearInvalidScriptsHandlers(const IScriptProvider &scriptProvider);
		void ClearSpecificScriptHandlers(const IScriptProvider &scriptProvider, ScriptID scriptID);
		void ClearScriptsHandlers();

	  protected:
		std::vector<EventHandler> &GetSortedHandlers();

		void ClearCaches();
		void ClearScriptHandlersImpl(std::function<bool(const EventHandler &)> pred);

		void InvokeFunction() noexcept;
	};
} // namespace tudov
