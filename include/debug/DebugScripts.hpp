#pragma once

#include "DebugElement.hpp"

#include <atomic>
#include <memory>
#include <thread>

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
	  	std::atomic<bool> isOpeningScriptEditor = false;
		std::thread _openScriptEditorThread;
		std::size_t _selectedIndex = -1;
		bool _autoScroll = true;
		char _filterText[128];

	  public:
		explicit DebugScripts() noexcept = default;
		~DebugScripts() noexcept;

		Log &GetLog() noexcept override;

		std::string_view GetName() noexcept override;
		void UpdateAndRender(IWindow &window) noexcept override;

	  private:
		void UpdateAndRenderLoadtimeErrorArea(IWindow &window) noexcept;
		void UpdateAndRenderRuntimeErrorArea(IWindow &window) noexcept;
	};
} // namespace tudov