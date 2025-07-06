#pragma once

#include "DebugConsole.h"
#include "DebugProfiler.h"
#include "IDebugElement.h"
#include "graphic/Renderer.h"
#include "util/Defs.h"

#include <memory>

namespace tudov
{
	class ScriptEngine;
	class Window;

	class DebugManager
	{
	  private:
		std::vector<std::shared_ptr<IDebugElement>> _elements;
		std::set<std::string_view> _shownElements;

	  public:
		std::weak_ptr<Window> window;
		std::shared_ptr<DebugConsole> console;
		std::shared_ptr<DebugProfiler> profiler;

	  private:
		void AddElement(const std::shared_ptr<IDebugElement> &element) noexcept;
		void RemoveElement(std::string_view element) noexcept;

	  public:
		explicit DebugManager(const std::weak_ptr<Window> &window) noexcept;

		void InstallToScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;
		void UninstallFromScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;

		void UpdateAndRender() noexcept;
	};
} // namespace tudov
