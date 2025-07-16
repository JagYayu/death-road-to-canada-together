#pragma once

#include "AbstractEvent.hpp"
#include "AddHandlerArgs.hpp"
#include "OverrideHandlerArgs.hpp"
#include "RuntimeEvent.hpp"
#include "util/Definitions.hpp"

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
		explicit LoadtimeEvent(IEventManager &eventManager, EventID eventID, ScriptID scriptID) noexcept;
		explicit LoadtimeEvent(IEventManager &eventManager, EventID eventID, ScriptID scriptID, const std::vector<std::string> &orders, const std::vector<EventHandleKey> &keys) noexcept;
		~LoadtimeEvent() noexcept override;

		virtual void Add(const AddHandlerArgs &handler) override;

		bool IsBuilt() const noexcept;
		/*
		 * return true if event already been built.
		 */
		bool TryBuild(ScriptID scriptID, const std::vector<std::string> &orders, const std::vector<EventHandleKey> &keys) noexcept;
		RuntimeEvent ToRuntime() noexcept;
	};
} // namespace tudov
