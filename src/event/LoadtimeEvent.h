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

		bool _built;
		std::vector<std::string> _orders;
		std::vector<EventHandleKey> _keys;
		std::vector<Operation> _operations;

	  public:
		explicit LoadtimeEvent(EventManager &eventManager, EventID eventID, ScriptID scriptID) noexcept;
		explicit LoadtimeEvent(EventManager &eventManager, EventID eventID, ScriptID scriptID, const std::vector<std::string> &orders, const std::vector<EventHandleKey> &keys) noexcept;
		~LoadtimeEvent() noexcept;

		virtual void Add(const AddHandlerArgs &handler) override;

		bool IsBuilt() const noexcept;
		/*
		 * return true if event already been built.
		 */
		bool TryBuild(ScriptID scriptID, const std::vector<std::string> &orders, const std::vector<EventHandleKey> &keys) noexcept;
		RuntimeEvent ToRuntime() noexcept;
	};
} // namespace tudov
