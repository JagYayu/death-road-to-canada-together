#pragma once

#include "EventHandler.hpp"
#include "util/Defs.h"

namespace tudov
{
	class RuntimeEvent;

	class AbstractEvent
	{
	  protected:
		String _scriptName;
		Vector<String> _orders;

		AbstractEvent(const String &scriptName);

	  public:
		virtual void Add(const EventHandler &handler) = 0;
	};
} // namespace tudov