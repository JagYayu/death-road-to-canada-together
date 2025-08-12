#pragma once

#include "DebugElement.hpp"

#include <memory>

namespace tudov
{
	class IWindow;

	class DebugScripts : public IDebugElement, public ILogProvider
	{
	  public:
		inline static constexpr std::string_view Name() noexcept
		{
			return "Scripts";
		}

	  public:
		std::weak_ptr<IWindow> window;

	  protected:
		std::size_t _selectedIndex = -1;
		bool _autoScroll = true;
		char _filterText[128];

	  public:
		explicit DebugScripts() noexcept = default;
		~DebugScripts() noexcept = default;

		Log &GetLog() noexcept override;

		std::string_view GetName() noexcept override;
		void UpdateAndRender(IWindow &window) noexcept override;

	  private:
		void UpdateAndRenderLoadtimeErrorArea(IWindow &window) noexcept;
		void UpdateAndRenderRuntimeErrorArea(IWindow &window) noexcept;
	};
} // namespace tudov