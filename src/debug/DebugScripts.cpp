#include "debug/DebugScripts.hpp"

#include "debug/DebugUtils.hpp"
#include "i18n/Localization.hpp"
#include "mod/ScriptErrors.hpp"
#include "mod/ScriptLoader.hpp"
#include "mod/ScriptProvider.hpp"
#include "resource/GlobalResourcesCollection.hpp"
#include "util/LogMicros.hpp"
#include "util/SystemUtils.hpp"

#include "imgui.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

using namespace tudov;

std::string_view DebugScripts::GetName() noexcept
{
	return Name();
}

DebugScripts::~DebugScripts() noexcept
{
	if (isOpeningScriptEditor && _openScriptEditorThread.joinable())
	{
		_openScriptEditorThread.join();
	}
}

Log &DebugScripts::GetLog() noexcept
{
	return *Log::Get("DebugScripts");
}

void DebugScripts::UpdateAndRender(IWindow &window) noexcept
{
	std::float_t scale = window.GetGUIScale();

	bool p_open = true;

	ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Scripts", &p_open))
	{
		if (ImGui::Button("Errors"))
		{
			//
		}
		ImGui::SameLine();
		if (ImGui::Button("Provided"))
		{
			//
		}
		ImGui::SameLine();
		if (ImGui::Button("Loaded"))
		{
			//
		}

		ImGui::Separator();

		ImGui::InputText("Script name filter", _filterText, IM_ARRAYSIZE(_filterText));

		if (window.GetScriptLoader().HasAnyLoadError())
		{
			UpdateAndRenderLoadtimeErrorArea(window);
		}
		else
		{
			UpdateAndRenderRuntimeErrorArea(window);
		}

		ImGui::End();
	}
}

void DebugScripts::UpdateAndRenderLoadtimeErrorArea(IWindow &window) noexcept
{
	if (!window.GetScriptLoader().HasAnyLoadError())
	{
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "Load Errors (0)");

		return;
	}

	ILocalization &localization = window.GetLocalization();
	IScriptProvider &scriptProvider = window.GetScriptProvider();

	auto &errors = window.GetScriptLoader().GetLoadErrorsCached();

	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Load Errors (%zu)", errors.size());

	if (ImGui::BeginChild("LoadErrors", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true))
	{
		std::float_t scale = window.GetGUIScale();

		ImGui::Columns(3, "loadErrorColumns");
		ImGui::SetColumnWidth(0, 200 * scale);
		ImGui::SetColumnWidth(1, 50 * scale);
		ImGui::SetColumnWidth(2, ImGui::GetWindowWidth() - 250 * scale);

		ImGui::Text("Script");
		ImGui::NextColumn();
		ImGui::Text("Line");
		ImGui::NextColumn();
		ImGui::Text("Error message");
		ImGui::NextColumn();

		ImGui::Separator();

		for (std::size_t index = 0; index < errors.size(); ++index)
		{
			auto &error = errors[index];

			std::string_view scriptName = scriptProvider.GetScriptNameByID(error->scriptID).value_or("");
			if (_filterText[0] != '\0' && scriptName.find(_filterText) == std::string::npos)
			{
				continue;
			}

			ImGui::Text("%s", scriptName.data());
			ImGui::NextColumn();
			ImGui::Text("%d", error->line);
			ImGui::NextColumn();

			bool selected = _selectedIndex == index;
			std::size_t labelEndline = -1;
			char *label = error->message.data();

			if (!selected)
			{
				for (std::size_t i = 0;; ++i)
				{
					char ch = label[i];
					if (ch == '0')
					{
						break;
					}
					if (ch == '\n')
					{
						labelEndline = i;
						label[i] = '\0';
						break;
					}
				}
			}

			if (ImGui::Selectable(label, selected, ImGuiSelectableFlags_SpanAllColumns))
			{
				_selectedIndex = selected ? -1 : index;
			}

			if (!selected && labelEndline != -1)
			{
				label[labelEndline] = '\n';
			}

			ImGui::NextColumn();
		}

		ImGui::Columns(1);
	}
	ImGui::EndChild();

	ScriptError *selectedError = nullptr;
	if (_selectedIndex >= 0 && _selectedIndex < errors.size())
	{
		auto &error = errors.at(_selectedIndex);
		if (error != nullptr)
		{
			selectedError = error.get();
		}
	}

	if (ImGui::Button("Open script editor") && selectedError != nullptr && SystemUtils::IsScriptEditorAvailable())
	{
		TextID textID = scriptProvider.GetScriptTextID(selectedError->scriptID);
		assert(textID != 0);

		std::string_view scriptPath = window.GetGlobalResourcesCollection().GetTextResources().GetResourcePath(textID);
		std::filesystem::path path = std::filesystem::current_path() / scriptPath;

		if (!isOpeningScriptEditor)
		{
			_openScriptEditorThread = SystemUtils::OpenScriptEditorAsync(path, selectedError->line, [this](const std::filesystem::path &path, std::uint32_t line)
			{
				TE_INFO("Opened file \"{}:{}\" with script editor", path.generic_string(), line);
				isOpeningScriptEditor = false;
			}, [this](const std::filesystem::path &path, std::uint32_t line, std::exception &e)
			{
				TE_WARN("Could not open file \"{}:{}\" with script editor: {}", path.generic_string(), line, e.what());
				isOpeningScriptEditor = false;
			});
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Copy selected to clipboard") && selectedError != nullptr)
	{
		SystemUtils::CopyToClipboard(selectedError->message);
	}

	ImGui::SameLine();

	if (ImGui::Button("Copy all to clipboard"))
	{
		//
	}
}

void DebugScripts::UpdateAndRenderRuntimeErrorArea(IWindow &window) noexcept
{
}
