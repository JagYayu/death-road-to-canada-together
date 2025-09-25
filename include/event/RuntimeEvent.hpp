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
#include "mod/ScriptEngine.hpp"
#include "system/Log.hpp"
#include "util/Definitions.hpp"

#include "sol/forward.hpp"
#include "sol/types.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

namespace tudov
{
	struct CoreEventData;
	struct IScriptEngine;
	struct IScriptProvider;
	class EventProfiler;

	class RuntimeEvent : public AbstractEvent, private ILogProvider
	{
		using InvocationCache = std::vector<EventHandler *>;
		using ProgressionID = std::uint32_t;

	  public:
		static std::vector<std::string> DefaultOrders;

	  private:
		struct Progression
		{
			std::size_t value;
			std::size_t total;
		};

		struct Profile
		{
			bool traceHandlers;
			std::unique_ptr<EventProfiler> eventProfiler;

			template <typename... TArgs>
			explicit Profile(bool traceHandlers, TArgs &&...args) noexcept
			    : traceHandlers(traceHandlers),
			      eventProfiler(std::make_unique<EventProfiler>(std::move<TArgs>(args)...))
			{
			}
		};

	  private:
		std::shared_ptr<Log> _log;
		std::shared_ptr<Profile> _profile;
		bool _hasAnyCache;
		bool _handlersSortedCache;
		std::optional<InvocationCache> _invocationCache;
		std::unordered_map<EventHandleKey, InvocationCache, EventHandleKey::Hash, EventHandleKey::Equal> _invocationCaches;
		std::string_view _defaultOrder;
		std::vector<std::string> _orders;
		std::unordered_set<EventHandleKey, EventHandleKey::Hash, EventHandleKey::Equal> _keys;
		std::vector<EventHandler> _handlers;
		ProgressionID _invocationTrackID;
		std::unordered_map<ProgressionID, Progression> _invocationTracks;
		ScriptID _invokingScriptID;

	  public:
		explicit RuntimeEvent(IEventManager &eventManager, EventID eventID, const std::vector<std::string> &orders = DefaultOrders, const std::unordered_set<EventHandleKey, EventHandleKey::Hash, EventHandleKey::Equal> &keys = {}, ScriptID scriptID = false);
		explicit RuntimeEvent(const RuntimeEvent &) noexcept = default;
		RuntimeEvent(RuntimeEvent &&) noexcept = default;
		RuntimeEvent &operator=(const RuntimeEvent &) noexcept = delete;
		RuntimeEvent &operator=(RuntimeEvent &&) noexcept = delete;
		~RuntimeEvent() noexcept override;

	  public:
		Log &GetLog() noexcept override;

		std::vector<EventHandler>::const_iterator BeginHandlers() const noexcept;
		std::vector<EventHandler>::const_iterator EndHandlers() const noexcept;

		[[nodiscard]] RuntimeEvent::Profile *GetProfile() const noexcept;
		void EnableProfiler(bool traceHandlers) noexcept;
		void DisableProfiler() noexcept;

		ProgressionID GetNextTrackID() const noexcept;
		std::tuple<std::size_t, std::size_t> GetProgression() const noexcept;
		ScriptID GetInvokingScriptID() const noexcept;

		/**
		 * Prefer using the another overload while this is mainly for internal usage.
		 */
		void Add(const AddHandlerArgs &args) override;
		void Add(const EventHandleFunction &function, std::string_view name, std::optional<std::string_view> order = std::nullopt, std::optional<EventHandleKey> key = std::nullopt, std::optional<std::double_t> sequence = std::nullopt);
		void Remove(std::string_view name);

		void Invoke(sol::object e = {}, const EventHandleKey &key = nullptr, EEventInvocation options = EEventInvocation::Default);
		[[deprecated]] void InvokeUncached(sol::object e = sol::lua_nil, const EventHandleKey &key = nullptr);

		void ClearInvalidScriptsHandlers(const IScriptProvider &scriptProvider);
		void ClearSpecificScriptHandlers(const IScriptProvider &scriptProvider, ScriptID scriptID);
		void ClearScriptsHandlers();

	  protected:
		std::vector<EventHandler> &GetSortedHandlers();

		void ClearCaches();
		void ClearScriptHandlersImpl(std::function<bool(const EventHandler &)> pred);

		void InvokeFunction() noexcept;

	  public:
		template <typename TData>
		void Invoke(TData *data, const EventHandleKey &key = nullptr, EEventInvocation options = EEventInvocation::Default) noexcept
		{
			if (data != nullptr)
			{
				sol::table args = GetScriptEngine().CreateTable(0, 1);
				args["data"] = data;
				Invoke(args, key, options);
			}
			else
			{
				Invoke(GetScriptEngine().CreateTable(), key, options);
			}
		}
	};
} // namespace tudov
