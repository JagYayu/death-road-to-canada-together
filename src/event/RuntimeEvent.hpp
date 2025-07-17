#pragma once

#include "AbstractEvent.hpp"
#include "EventHandler.hpp"
#include "debug/EventProfiler.hpp"
#include "util/Definitions.hpp"
#include "util/Log.hpp"

#include "sol/forward.hpp"
#include "sol/types.hpp"

#include <optional>
#include <unordered_set>

namespace tudov
{
	struct CoreEventData;
	struct IEventProfiler;
	struct IScriptEngine;
	struct IScriptProvider;

	class RuntimeEvent : public AbstractEvent
	{
		using TInvocationCache = std::vector<EventHandler *>;
		using TInvocationTrackID = std::uint32_t;

	  public:
		enum class EInvocation
		{
			Default = 0,
			CacheHandlers = 1 << 0,
			NoProfiler = 1 << 1,
			TrackProgression = 1 << 2,
		};

	  private:
		struct InvocationTrack
		{
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
		std::optional<TInvocationCache> _invocationCache;
		std::unordered_map<EventHandleKey, TInvocationCache, EventHandleKey::Hash, EventHandleKey::Equal> _invocationCaches;
		std::vector<std::string> _orders;
		std::unordered_set<EventHandleKey, EventHandleKey::Hash, EventHandleKey::Equal> _keys;
		std::vector<EventHandler> _handlers;
		TInvocationTrackID _invocationTrackID;

	  public:
		explicit RuntimeEvent(IEventManager &eventManager, EventID eventID, const std::vector<std::string> &orders = {""}, const std::unordered_set<EventHandleKey, EventHandleKey::Hash, EventHandleKey::Equal> &keys = {}, ScriptID scriptID = false);
		~RuntimeEvent() noexcept override;

	  private:
		void ClearCaches();
		void ClearScriptHandlersImpl(std::function<bool(const EventHandler &)> pred);

		void InvokeFunction() noexcept;

	  protected:
		std::vector<EventHandler> &GetSortedHandlers();

	  public:
		std::vector<EventHandler>::const_iterator BeginHandlers() const noexcept;
		std::vector<EventHandler>::const_iterator EndHandlers() const noexcept;

		std::optional<std::reference_wrapper<RuntimeEvent::Profile>> GetProfile() const noexcept;
		void EnableProfiler(bool traceHandlers) noexcept;
		void DisableProfiler() noexcept;

		void Add(const AddHandlerArgs &args) override;

		void Invoke(const sol::object &e = sol::lua_nil, const EventHandleKey &key = {}, EInvocation options = EInvocation::Default);
		[[deprecated]] void InvokeUncached(const sol::object &e = sol::lua_nil, const EventHandleKey &key = {});

		void Invoke(IScriptEngine &scriptEngine, CoreEventData *data, const EventHandleKey &key = {}, EInvocation options = EInvocation::Default);

		TInvocationTrackID GetNextInvocationID() noexcept;

		void ClearInvalidScriptsHandlers(const IScriptProvider &scriptProvider);
		void ClearSpecificScriptHandlers(const IScriptProvider &scriptProvider, ScriptID scriptID);
		void ClearScriptsHandlers();
	};
} // namespace tudov
