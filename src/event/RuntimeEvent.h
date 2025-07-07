#pragma once

#include "AbstractEvent.h"
#include "debug/EventProfiler.h"
#include "util/Defs.h"
#include "util/Log.h"

#include <optional>
#include <sol/forward.hpp>
#include <sol/types.hpp>

#include <string>
#include <variant>
#include <vector>

namespace tudov
{
	class EventProfiler;
	class IScriptProvider;

	class RuntimeEvent : public AbstractEvent
	{
		using InvocationCache = std::vector<EventHandleFunction>;

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
		std::unique_ptr<Profile> _profile;
		bool _handlersSortedCache;
		std::optional<InvocationCache> _invocationCache;
		std::unordered_map<EventHandleKey, InvocationCache, EventHandleKey::Hash, EventHandleKey::Equal> _invocationCaches;
		std::vector<std::string> _orders;
		std::unordered_set<EventHandleKey, EventHandleKey::Hash, EventHandleKey::Equal> _keys;
		std::vector<EventHandler> _handlers;

	  public:
		explicit RuntimeEvent(EventManager &eventManager, EventID eventID, const std::vector<std::string> &orders = {""}, const std::unordered_set<EventHandleKey, EventHandleKey::Hash, EventHandleKey::Equal> &keys = {}, ScriptID scriptID = false);

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

		void Invoke(const sol::object &args = sol::lua_nil, EventHandleKey key = {});
		void InvokeUncached(const sol::object &args = sol::lua_nil, EventHandleKey key = {});

		void ClearInvalidScriptsHandlers(const IScriptProvider &scriptProvider);
		void ClearSpecificScriptHandlers(const IScriptProvider &scriptProvider, ScriptID scriptID);
		void ClearScriptsHandlers();
	};
} // namespace tudov
