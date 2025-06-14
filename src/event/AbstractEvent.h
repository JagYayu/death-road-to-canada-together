#pragma once

#include "AddHandlerArgs.hpp"
#include "util/Defs.h"

namespace tudov
{
	class EventManager;
	class RuntimeEvent;

	class AbstractEvent
	{
	  protected:
		EventID _id;
		ScriptID _scriptID;

	  public:
		EventManager &eventManager;

	  protected:
		explicit AbstractEvent(EventManager &eventManager, EventID eventID, ScriptID scriptID = false);

	  public:
		void IsValid() const noexcept;
		EventID GetID() const noexcept;
		ScriptID GetScriptID() const noexcept;

		virtual void Add(const AddHandlerArgs &args) = 0;
	};
} // namespace tudov
