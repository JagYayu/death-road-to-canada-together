/**
 * @file debug/DebugScripts.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "debug/DebugScripts.hpp"

#include "debug/DebugUtils.hpp"
#include "event/CoreEvents.hpp"
#include "event/EventInvocation.hpp"
#include "event/EventManager.hpp"
#include "event/RuntimeEvent.hpp"
#include "i18n/Localization.hpp"
#include "mod/ScriptEngine.hpp"
#include "mod/ScriptErrors.hpp"
#include "mod/ScriptLoader.hpp"
#include "mod/ScriptModule.hpp"
#include "mod/ScriptProvider.hpp"
#include "resource/GlobalResourcesCollection.hpp"
#include "system/LogMicros.hpp"
#include "util/SystemUtils.hpp"

#include "imgui.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

using namespace tudov;

std::string_view DebugScripts::GetName() noexcept
{
	return Name();
}

DebugScripts::~DebugScripts() noexcept
{
	if (_isOpeningScriptEditor && _openScriptEditorThread.joinable())
	{
		_openScriptEditorThread.join();
	}
}

Log &DebugScripts::GetLog() noexcept
{
	return *Log::Get("DebugScripts");
}

void DebugScripts::OnOpened(IWindow &window) noexcept
{
	UpdateCaches(window);
}

void DebugScripts::OnClosed(IWindow &window) noexcept
{
	_providedScriptsVersionID = 0;
	_providedScriptsCache = {};
}

void DebugScripts::UpdateCaches(IWindow &window) noexcept
{
	IScriptProvider &scriptProvider = window.GetScriptProvider();

	_providedScriptsCache.clear();

	for (const auto &entry : scriptProvider)
	{
		_providedScriptsCache.emplace_back(entry.scriptID, entry.name);
	}

	std::sort(_providedScriptsCache.begin(), _providedScriptsCache.end(), [](const LoadedScriptEntry &l, const LoadedScriptEntry &r) -> bool
	{
		return l.name < r.name;
	});
}

void DebugScripts::UpdateAndRender(IWindow &window) noexcept
{
	if (_providedScriptsVersionID != window.GetScriptProvider().GetVersionID()) [[unlikely]]
	{
		UpdateCaches(window);
		_providedScriptsVersionID = window.GetScriptProvider().GetVersionID();
	}

	std::float_t scale = window.GetGUIScale();

	ImGui::SetNextWindowSize(ImVec2(800 * scale, 600 * scale), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Scripts"))
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
				UpdateAndRenderProvidedScripts(window);

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
		// ImGui::SetColumnWidth(0, 100);
		// ImGui::SetColumnWidth(1, 200);
		// ImGui::SetColumnWidth(2, 50);
		// ImGui::SetColumnWidth(3, ImGui::GetWindowWidth() - 300);

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
		if (textID != 0 && !_isOpeningScriptEditor)
		{
			std::string_view scriptPath = window.GetGlobalResourcesCollection().GetTextResources().GetResourcePath(textID);
			std::filesystem::path path = std::filesystem::current_path() / scriptPath;

			_openScriptEditorThread = SystemUtils::OpenScriptEditorAsync(path, selectedError->line, [this](const std::filesystem::path &path, std::uint32_t line)
			{
				TE_INFO("Opened file \"{}:{}\" with script editor", path.generic_string(), line);
				_isOpeningScriptEditor = false;
			}, [this](const std::filesystem::path &path, std::uint32_t line, std::exception &e)
			{
				TE_WARN("Could not open file \"{}:{}\" with script editor: {}", path.generic_string(), line, e.what());
				_isOpeningScriptEditor = false;
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
	clipper.Begin(static_cast<int>(_providedScriptsCache.size()));

	while (clipper.Step())
	{
		auto it = _providedScriptsCache.begin();
		std::advance(it, clipper.DisplayStart);

		for (std::size_t index = clipper.DisplayStart; index < clipper.DisplayEnd && it != _providedScriptsCache.end(); ++index, ++it)
		{
			ScriptID scriptID = it->id;
			std::string_view scriptName = it->name;

			// Column 1: Script Name
			ImGui::TextUnformatted(scriptName.data());
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

			if (ImGui::Button(std::format("Reload##{}", scriptID).data()))
			{
				std::vector<ScriptID> scriptIDs = scriptLoader.GetScriptDependencies(scriptID);
				scriptIDs.emplace_back(scriptID);
				scriptLoader.HotReloadScripts(scriptIDs);
			}

			ImGui::SameLine();

			if (ImGui::Button(std::format("{}##{}", "Print", scriptID).data()))
			{
				TE_DEBUG("Inspect script <{}>\"{}\": 'module', 'snapshot', 'persists'", scriptID, scriptName);

				IScriptEngine &scriptEngine = window.GetScriptEngine();
				std::string clipboardText;

				{
					sol::table moduleTable = scriptLoader.Load(scriptID)->GetTable();
					std::string text = scriptEngine.Inspect(moduleTable);

					TE_INFO("{}", text);
					clipboardText.append(text);
				}

				clipboardText.append("\n");

				{
					sol::table snapshot = scriptEngine.CreateTable();
					eventDebugSnapshot.Invoke(snapshot, scriptName.data(), EEventInvocation::None);
					std::string text = scriptEngine.Inspect(snapshot);

					TE_INFO("{}", text);
					clipboardText.append(text);
				}

				clipboardText.append("\n");

				{
					sol::table persistVariables = scriptEngine.CreateTable();
					scriptEngine.SaveScriptPersistVariables(scriptName);
					for (const auto &[key, value] : scriptEngine.GetScriptPersistVariables(scriptName))
					{
						persistVariables[key] = value;
					}
					std::string text = scriptEngine.Inspect(persistVariables);

					TE_INFO("{}", text);
					clipboardText.append(text);
				}

				SystemUtils::CopyToClipboard(clipboardText);
			}

			ImGui::NextColumn();
		}
	}

	ImGui::Columns(1); // Reset columns
}
