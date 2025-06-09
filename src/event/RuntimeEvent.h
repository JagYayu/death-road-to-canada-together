#pragma once

#include "AbstractEvent.h"
#include "EventHandler.hpp"
#include "sol/forward.hpp"
#include "util/Defs.h"

#include <string>
#include <variant>
#include <vector>

namespace tudov
{
	class RuntimeEvent : public AbstractEvent
	{
		using InvocationCache = Vector<EventHandler::Function>;

	  private:
		Optional<InvocationCache> _invocationCache;
		UnorderedMap<EventHandler::Key, InvocationCache, EventHandler::KeyHash, EventHandler::KeyEqual> _invocationCaches;
		Vector<String> _keys;
		Vector<EventHandler> _handlers;
		Bool _handlersSorted;

	  protected:
		void Add(const EventHandler &handler) override;

		Vector<EventHandler> &GetSortedHandlers();

		void Invoke(const sol::object &args, Optional<EventHandler::Key> key = null);
		void InvokeUncached(const sol::object &args, Optional<EventHandler::Key> key = null);
	};
} // namespace tudov
