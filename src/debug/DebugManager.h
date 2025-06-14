#pragma once

#include "DebugConsole.h"
#include "DebugProfiler.h"
#include "IDebugElement.h"
#include "util/Defs.h"

namespace tudov
{
	class ScriptEngine;
	class Window;

	class DebugManager
	{
	  private:
		Vector<SharedPtr<IDebugElement>> _elements;
		Set<StringView> _shownElements;

	  public:
		Window &window;
		SharedPtr<DebugConsole> console;
		SharedPtr<DebugProfiler> profiler;

	  private:
		void AddElement(const SharedPtr<IDebugElement> &element) noexcept;
		void RemoveElement(StringView element) noexcept;

	  public:
		explicit DebugManager(Window &window) noexcept;

		void RegisterGlobals(StringView name, ScriptEngine &scriptEngine) noexcept;
		void UnregisterGlobals(StringView name, ScriptEngine &scriptEngine) noexcept;

		void UpdateAndRender() noexcept;
	};
} // namespace tudov
