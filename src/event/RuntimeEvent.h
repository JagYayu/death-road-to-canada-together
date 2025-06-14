#pragma once

#include "AbstractEvent.h"
#include "debug/EventProfiler.h"
#include "util/Defs.h"
#include "util/Log.h"

#include <sol/forward.hpp>
#include <sol/types.hpp>

#include <string>
#include <variant>
#include <vector>

namespace tudov
{
	class EventProfiler;
	class ScriptProvider;

	class RuntimeEvent : public AbstractEvent
	{
		using InvocationCache = Vector<EventHandler::Function>;

		struct Profile
		{
			bool traceHandlers;
			EventProfiler eventProfiler;

			template <typename... TArgs>
			explicit Profile(bool traceHandlers, TArgs &&...args) noexcept
			    : traceHandlers(traceHandlers),
			      eventProfiler(Move<TArgs>(args)...)
			{
			}
		};

	  private:
		SharedPtr<Log> _log;
		UniquePtr<Profile> _profile;
		bool _handlersSortedCache;
		Optional<InvocationCache> _invocationCache;
		UnorderedMap<EventHandler::Key, InvocationCache, EventHandler::Key::Hash, EventHandler::Key::Equal> _invocationCaches;
		Vector<String> _orders;
		UnorderedSet<EventHandler::Key, EventHandler::Key::Hash, EventHandler::Key::Equal> _keys;
		Vector<EventHandler> _handlers;

	  public:
		explicit RuntimeEvent(EventManager &eventManager, EventID eventID, const Vector<String> &orders = {""}, const UnorderedSet<EventHandler::Key, EventHandler::Key::Hash, EventHandler::Key::Equal> &keys = {}, ScriptID scriptID = false);

	  private:
		void ClearCaches();
		void ClearScriptHandlersImpl(Function<bool(const EventHandler &)> pred);

	  protected:
		Vector<EventHandler> &GetSortedHandlers();

	  public:
		Optional<Reference<RuntimeEvent::Profile>> GetProfile() const noexcept;
		void EnableProfiler(bool traceHandlers) noexcept;
		void DisableProfiler() noexcept;

		void Add(const AddHandlerArgs &args) override;

		void Invoke(const sol::object &args = sol::lua_nil, Optional<EventHandler::Key> key = nullopt);
		void InvokeUncached(const sol::object &args = sol::lua_nil, Optional<EventHandler::Key> key = nullopt);

		void ClearInvalidScriptsHandlers(const ScriptProvider &scriptProvider);
		void ClearScriptsHandlers();
	};
} // namespace tudov
