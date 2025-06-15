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
		std::vector<SharedPtr<IDebugElement>> _elements;
		std::set<std::string_view> _shownElements;

	  public:
		Window &window;
		SharedPtr<DebugConsole> console;
		SharedPtr<DebugProfiler> profiler;

	  private:
		void AddElement(const SharedPtr<IDebugElement> &element) noexcept;
		void RemoveElement(std::string_view element) noexcept;

	  public:
		explicit DebugManager(Window &window) noexcept;

		void RegisterGlobalsTo(std::string_view name, ScriptEngine &scriptEngine) noexcept;
		void UnregisterGlobalsFrom(std::string_view name, ScriptEngine &scriptEngine) noexcept;

		void UpdateAndRender() noexcept;
	};
} // namespace tudov
