#pragma once

#include "AbstractEvent.h"
#include "AddHandlerArgs.hpp"
#include "OverrideHandlerArgs.hpp"
#include "RuntimeEvent.h"
#include "util/Defs.h"

namespace tudov
{
	class LoadtimeEvent : public AbstractEvent
	{
	  protected:
		using Operation = Variant<AddHandlerArgs, OverrideHandlerArgs>;

		Vector<String> _orders;
		UnorderedSet<EventHandler::Key, EventHandler::Key::Hash, EventHandler::Key::Equal> _keys;
		Vector<Operation> _operations;

	  public:
		virtual void Add(const AddHandlerArgs &handler) override;

		RuntimeEvent ToRuntime();
	};
} // namespace tudov
