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
		Vector<EventHandler::Key> _keys;
		Vector<Operation> _operations;

	  public:
		explicit LoadtimeEvent(EventManager &eventManager, EventID eventID, const Vector<String> &orders, const Vector<EventHandler::Key> &keys, ScriptID scriptID) noexcept;

		virtual void Add(const AddHandlerArgs &handler) override;

		RuntimeEvent ToRuntime() noexcept;
	};
} // namespace tudov
