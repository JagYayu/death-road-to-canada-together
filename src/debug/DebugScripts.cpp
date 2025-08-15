#include "debug/DebugScripts.hpp"

#include "debug/DebugUtils.hpp"
#include "event/EventManager.hpp"
#include "i18n/Localization.hpp"
#include "mod/ScriptErrors.hpp"
#include "mod/ScriptLoader.hpp"
#include "mod/ScriptModule.hpp"
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

	ImGui::SetNextWindowSize(ImVec2(800 * scale, 600 * scale), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Scripts", &p_open))
	{
		ImGui::InputText("Script name filter", _filterText, IM_ARRAYSIZE(_filterText));

		if (ImGui::BeginTabBar("PagesTab"))
		{
			if (ImGui::BeginTabItem("Errors"))
			{
				IScriptErrors &scriptErrors = window.GetScriptErrors();
				if (scriptErrors.HasLoadtimeError())
				{
					ErrorsArea errorsArea{
					    "LoadErrors",
					    "Loadtime Errors",
					    scriptErrors.GetLoadtimeErrorsCached(),
					    _filterText,
					};
					UpdateAndRenderErrorsArea(window, errorsArea);
				}
				else if (scriptErrors.HasRuntimeError())
				{
					ErrorsArea errorsArea{
					    "RuntimeErrors",
					    "Runtime Errors",
					    scriptErrors.GetRuntimeErrorsCached(),
					    _filterText,
					};
					UpdateAndRenderErrorsArea(window, errorsArea);
				}
				else
				{
					ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "No script errors");
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Loaded"))
			{
				UpdateAndRenderLoadedScripts(window);

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
}

void DebugScripts::UpdateAndRenderErrorsArea(IWindow &window, const ErrorsArea &errorsArea) noexcept
{
	ILocalization &localization = window.GetLocalization();
	IScriptProvider &scriptProvider = window.GetScriptProvider();

	auto &errors = errorsArea.errors;

	ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s: %zu", errorsArea.name.data(), errors.size());

	if (ImGui::BeginChild(errorsArea.id.data(), ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true))
	{
		std::float_t scale = window.GetGUIScale();

		ImGui::Columns(4, "loadErrorColumns");
		ImGui::SetColumnWidth(0, 100 * scale);
		ImGui::SetColumnWidth(1, 200 * scale);
		ImGui::SetColumnWidth(2, 50 * scale);
		ImGui::SetColumnWidth(3, ImGui::GetWindowWidth() - 300 * scale);

		ImGui::Text("Time");
		ImGui::NextColumn();
		ImGui::Text("Script");
		ImGui::NextColumn();
		ImGui::Text("Line");
		ImGui::NextColumn();
		ImGui::Text("Error message");
		ImGui::NextColumn();

		ImGui::Separator();

		for (std::size_t index = 0; index < errors.size(); ++index)
		{
			ImGui::PushID(index);

			auto &error = errors[index];

			ImGui::Text("%s", std::format("{:%H:%M:%S}", error->time).data());
			ImGui::NextColumn();

			std::string_view scriptName = scriptProvider.GetScriptNameByID(error->scriptID).value_or("$UNKNOWN$");
			if (_filterText[0] != '\0' && scriptName.find(_filterText) == std::string::npos)
			{
				continue;
			}

			ImGui::Text("%s", scriptName.data());
			ImGui::NextColumn();
			ImGui::Text("%d", error->line);
			ImGui::NextColumn();

			bool selected = _selectedLoadtimeErrorIndex == index;
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
				_selectedLoadtimeErrorIndex = selected ? -1 : index;
			}

			if (!selected && labelEndline != -1)
			{
				label[labelEndline] = '\n';
			}

			ImGui::NextColumn();

			ImGui::PopID();
		}
	}
	ImGui::EndChild();

	ScriptError *selectedError = nullptr;
	if (_selectedLoadtimeErrorIndex >= 0 && _selectedLoadtimeErrorIndex < errors.size())
	{
		auto &error = errors.at(_selectedLoadtimeErrorIndex);
		if (error != nullptr)
		{
			selectedError = error.get();
		}
	}

	if (ImGui::Button("Open script editor") && selectedError != nullptr && SystemUtils::IsScriptEditorAvailable())
	{
		TextID textID = scriptProvider.GetScriptTextID(selectedError->scriptID);
		if (textID != 0 && !isOpeningScriptEditor)
		{
			std::string_view scriptPath = window.GetGlobalResourcesCollection().GetTextResources().GetResourcePath(textID);
			std::filesystem::path path = std::filesystem::current_path() / scriptPath;

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

void DebugScripts::UpdateAndRenderProvidedScripts(IWindow &window) noexcept
{
}

void DebugScripts::UpdateAndRenderLoadedScripts(IWindow &window) noexcept
{
	IScriptProvider &scriptProvider = window.GetScriptProvider();
	IScriptLoader &scriptLoader = window.GetScriptLoader();
	RuntimeEvent &eventDebugSnapshot = window.GetEventManager().GetCoreEvents().DebugSnapshot();

	ImGui::Columns(3, "ScriptColumns", true);
	// ImGui::SetColumnWidth(0, 100 * scale);
	// ImGui::SetColumnWidth(1, 200 * scale);
	// ImGui::SetColumnWidth(3, ImGui::GetWindowWidth() - 300 * scale);

	// Column headers
	ImGui::Text("Script");
	ImGui::NextColumn();
	ImGui::Text("Status");
	ImGui::NextColumn();
	ImGui::Text("Actions");
	ImGui::NextColumn();

	ImGui::Separator();

	// Cache some styles for performance
	constexpr ImVec4 successColor{0.0f, 1.0f, 0.0f, 1.0f};
	constexpr ImVec4 errorColor{1.0f, 0.0f, 0.0f, 1.0f};
	constexpr ImVec4 warningColor{1.0f, 1.0f, 0.0f, 1.0f};

	// Use clipper for large lists to improve performance
	ImGuiListClipper clipper{};
	clipper.Begin(static_cast<int>(scriptProvider.GetEntriesSize()));

	while (clipper.Step())
	{
		auto it = scriptProvider.begin();
		std::advance(it, clipper.DisplayStart);

		for (std::size_t index = clipper.DisplayStart; index < clipper.DisplayEnd && it != scriptProvider.end(); ++index, ++it)
		{
			auto &[scriptID, entry] = *it;

			// Column 1: Script Name
			ImGui::TextUnformatted(entry.name.c_str());
			ImGui::NextColumn();

			// Column 2: Load Status
			if (scriptLoader.IsScriptFullyLoaded(scriptID))
			{
				ImGui::TextColored(successColor, "Fully loaded");
			}
			else if (scriptLoader.IsScriptLazyLoaded(scriptID))
			{
				ImGui::TextColored(warningColor, "Lazy loaded");
			}
			else if (scriptLoader.IsScriptValid(scriptID))
			{
				ImGui::TextColored(errorColor, "Load error");
			}
			else if (!scriptLoader.IsScriptExists(scriptID))
			{
				ImGui::TextColored(warningColor, "Not exist");
			}
			else
			{
				ImGui::TextColored(warningColor, "Unknown state");
			}

			ImGui::NextColumn();

			if (ImGui::Button(std::format("Module##{}", scriptID).data()))
			{
				IScriptEngine &scriptEngine = window.GetScriptEngine();

				sol::table moduleTable = scriptLoader.Load(scriptID)->GetTable();
				std::string text = scriptEngine.Inspect(moduleTable);

				SystemUtils::CopyToClipboard(text);

				TE_DEBUG("Inspect script module <{}>\"{}\"", scriptID, entry.name);
				TE_INFO("{}", text);
			}
			ImGui::SameLine();
			if (ImGui::Button(std::format("Snapshot##{}", scriptID).data()))
			{
				IScriptEngine &scriptEngine = window.GetScriptEngine();

				sol::table moduleSnapshot = scriptEngine.CreateTable();
				eventDebugSnapshot.Invoke(moduleSnapshot, entry.name.data(), RuntimeEvent::EInvocation::None);
				std::string text = scriptEngine.Inspect(moduleSnapshot);

				SystemUtils::CopyToClipboard(text);

				TE_DEBUG("Inspect script snapshot <{}>\"{}\"", scriptID, entry.name);
				TE_INFO("{}", text);
			}

			ImGui::NextColumn();
		}
	}

	ImGui::Columns(1); // Reset columns
}
