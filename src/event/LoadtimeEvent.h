#pragma once

#include "AbstractEvent.h"
#include "RuntimeEvent.h"

namespace tudov
{
	class LoadtimeEvent : AbstractEvent
	{
	  public:
		RuntimeEvent ToRuntime();
	};
} // namespace tudov
