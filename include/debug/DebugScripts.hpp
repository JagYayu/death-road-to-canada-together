/**
 * @file debug/DebugScripts.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "DebugElement.hpp"
#include "util/Definitions.hpp"

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

namespace tudov
{
	struct IWindow;
	class ScriptError;

	class DebugScripts : public IDebugElement, private ILogProvider
	{
	  protected:
		struct ErrorsArea
		{
			std::string_view id;
			std::string_view name;
			const std::vector<std::shared_ptr<ScriptError>> &errors;
			std::string_view filter;
		};

		struct LoadedScriptEntry
		{
			ScriptID id;
			std::string_view name;
		};

	  public:
		inline static constexpr std::string_view Name() noexcept
		{
			return "Scripts";
		}

	  public:
		std::weak_ptr<IWindow> window;

	  protected:
		std::atomic<bool> _isOpeningScriptEditor = false;
		std::thread _openScriptEditorThread;
		std::size_t _selectedLoadtimeErrorIndex = -1;
		std::size_t _selectedRuntimeErrorIndex = -1;
		bool _autoScroll = true;
		char _filterText[128];

		std::uint64_t _providedScriptsVersionID = 0;
		std::vector<LoadedScriptEntry> _providedScriptsCache;

	  public:
		explicit DebugScripts() noexcept = default;
		~DebugScripts() noexcept;

		Log &GetLog() noexcept override;

		std::string_view GetName() noexcept override;
		void OnOpened(IWindow &window) noexcept override;
		void OnClosed(IWindow &window) noexcept override;
		void UpdateAndRender(IWindow &window) noexcept override;

	  private:
		void UpdateCaches(IWindow &window) noexcept;

		void UpdateAndRenderErrorsArea(IWindow &window, const ErrorsArea &errorsArea) noexcept;
		void UpdateAndRenderProvidedScripts(IWindow &window) noexcept;
		void UpdateAndRenderDependencies(IWindow &window) noexcept;
	};
} // namespace tudov