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
	class RuntimeEvent : public AbstractEvent
	{
		using InvocationCache = Vector<EventHandler::Function>;

	  private:
		SharedPtr<Log> _log;
		bool _handlersSortedCache;
		Optional<InvocationCache> _invocationCache;
		UnorderedMap<EventHandler::Key, InvocationCache, EventHandler::KeyHash, EventHandler::KeyEqual> _invocationCaches;
		Vector<String> _keys;
		Vector<EventHandler> _handlers;

	  public:
		explicit RuntimeEvent(const String &name);
		RuntimeEvent(const String &scriptName, const String &name);

	  private:
		void ClearScriptHandlersImpl(Function<bool(const EventHandler &)> pred);

	  protected:
		void ClearCaches();

		Vector<EventHandler> &GetSortedHandlers();

	  public:
		void Add(const AddHandlerArgs &args) override;

		void Invoke(const sol::object &args = sol::lua_nil, Optional<EventHandler::Key> key = null);
		void InvokeUncached(const sol::object &args = sol::lua_nil, Optional<EventHandler::Key> key = null);

		void ClearScriptHandlers(const String &scriptName);
		void ClearScriptsHandlers();
	};
} // namespace tudov
