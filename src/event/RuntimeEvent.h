#pragma once

#include "AbstractEvent.h"
#include "sol/forward.hpp"
#include "sol/types.hpp"
#include "util/Defs.h"
#include "util/Log.h"

#include <string>
#include <variant>
#include <vector>

namespace tudov
{
	class ScriptProvider;

	class RuntimeEvent : public AbstractEvent
	{
		using InvocationCache = Vector<EventHandler::Function>;

	  private:
		SharedPtr<Log> _log;
		bool _handlersSortedCache;
		Optional<InvocationCache> _invocationCache;
		UnorderedMap<EventHandler::Key, InvocationCache, EventHandler::Key::Hash, EventHandler::Key::Equal> _invocationCaches;
		Vector<String> _orders;
		UnorderedSet<EventHandler::Key, EventHandler::Key::Hash, EventHandler::Key::Equal> _keys;
		Vector<EventHandler> _handlers;

	  public:
		explicit RuntimeEvent(EventID eventID, const Vector<String> &orders = {""}, const UnorderedSet<EventHandler::Key, EventHandler::Key::Hash, EventHandler::Key::Equal> &keys = {}, ScriptID scriptID = false);

	  private:
		void ClearCaches();
		void ClearScriptHandlersImpl(Function<bool(const EventHandler &)> pred);

	  protected:
		Vector<EventHandler> &GetSortedHandlers();

	  public:
		void Add(const AddHandlerArgs &args) override;

		void Invoke(const sol::object &args = sol::lua_nil, Optional<EventHandler::Key> key = null);
		void InvokeUncached(const sol::object &args = sol::lua_nil, Optional<EventHandler::Key> key = null);

		void ClearInvalidScriptsHandlers(const ScriptProvider &scriptProvider);
		void ClearScriptsHandlers();
	};
} // namespace tudov
