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

		Vector<Operation> operations;

	  public:
		virtual void Add(const AddHandlerArgs &handler) override;

		RuntimeEvent ToRuntime();
	};
} // namespace tudov
