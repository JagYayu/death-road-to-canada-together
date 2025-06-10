#pragma once

#include "AddHandlerArgs.hpp"
#include "util/Defs.h"

namespace tudov
{
	class RuntimeEvent;

	class AbstractEvent
	{
	  protected:
		String _scriptName;
		String _name;
		Vector<String> _orders;

		AbstractEvent(const String &scriptName, const String &name);

	  public:
		const String &GetName() const noexcept;
		const String &GetScriptName() const noexcept;

		virtual void Add(const AddHandlerArgs &args) = 0;
	};
} // namespace tudov
