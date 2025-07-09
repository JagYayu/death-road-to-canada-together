#pragma once

#include "DebugConsole.hpp"
#include "DebugProfiler.hpp"
#include "IDebugElement.hpp"

#include <memory>
#include <set>

namespace tudov
{
	struct IWindow;

	class DebugManager
	{
	  private:
		std::vector<std::shared_ptr<IDebugElement>> _elements;
		std::set<std::string_view> _shownElements;

	  public:
		std::shared_ptr<DebugConsole> console;
		std::shared_ptr<DebugProfiler> profiler;

	  private:
		void AddElement(const std::shared_ptr<IDebugElement> &element) noexcept;
		void RemoveElement(std::string_view element) noexcept;

	  public:
		explicit DebugManager() noexcept;

		void UpdateAndRender(const std::shared_ptr<IWindow> &window) noexcept;
	};
} // namespace tudov
