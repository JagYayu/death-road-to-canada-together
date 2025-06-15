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
		using Operation = std::variant<AddHandlerArgs, OverrideHandlerArgs>;

		std::vector<std::string> _orders;
		std::vector<EventHandler::Key> _keys;
		std::vector<Operation> _operations;

	  public:
		explicit LoadtimeEvent(EventManager &eventManager, EventID eventID, const std::vector<std::string> &orders, const std::vector<EventHandler::Key> &keys, ScriptID scriptID) noexcept;

		virtual void Add(const AddHandlerArgs &handler) override;

		RuntimeEvent ToRuntime() noexcept;
	};
} // namespace tudov
