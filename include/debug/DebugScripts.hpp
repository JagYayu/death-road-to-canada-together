#pragma once

#include "DebugElement.hpp"

#include <atomic>
#include <memory>
#include <thread>

namespace tudov
{
	class IWindow;

	class DebugScripts : public IDebugElement, private ILogProvider
	{
	  protected:
		enum class EPage
		{
			Errors,
			Provided,
			Loaded,

			Default = Errors,
		};

		struct ErrorsArea
		{
			std::string_view id;
			std::string_view name;
			const std::vector<std::shared_ptr<ScriptError>> &errors;
			std::string_view filter;
		};

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
		std::size_t _selectedLoadtimeErrorIndex = -1;
		std::size_t _selectedRuntimeErrorIndex = -1;
		bool _autoScroll = true;
		char _filterText[128];
		EPage _page = DebugScripts::EPage::Default;

	  public:
		explicit DebugScripts() noexcept = default;
		~DebugScripts() noexcept;

		Log &GetLog() noexcept override;

		std::string_view GetName() noexcept override;
		void UpdateAndRender(IWindow &window) noexcept override;

	  private:
		void UpdateAndRenderErrorsArea(IWindow &window, const ErrorsArea &errorsArea) noexcept;
		void UpdateAndRenderProvidedScripts(IWindow &window) noexcept;
		void UpdateAndRenderLoadedScripts(IWindow &window) noexcept;
	};
} // namespace tudov