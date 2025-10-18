/**
 * @file debug/DebugLog.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Debug/DebugLog.hpp"
#include "Util/EnumFlag.hpp"

#include "imgui.h"

#include <tuple>
#include <vector>

using namespace tudov;

DebugLog::DebugLog() noexcept
{
}

std::string_view DebugLog::GetName() noexcept
{
	return Name();
}

void DebugLog::UpdateAndRender(IWindow &window) noexcept
{
	static auto TableLogFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg;
	static auto VerbosityLevels = std::vector<std::tuple<ELogVerbosity, std::string_view>>{
	    {ELogVerbosity::Error, "Error"},
	    {ELogVerbosity::Warn, "Warn"},
	    {ELogVerbosity::Info, "Info"},
	    {ELogVerbosity::Debug, "Debug"},
	    {ELogVerbosity::Trace, "Trace"},
	    {ELogVerbosity::All, "All"},
	};

	if (ImGui::BeginTable("Log", VerbosityLevels.size() + 1, TableLogFlags))
	{
		ImGui::TableSetupColumn("Module", ImGuiTableColumnFlags_WidthFixed, 150.0f);
		for (auto &&[verb, str] : VerbosityLevels)
		{
			ImGui::TableSetupColumn(str.data());
		}
		ImGui::TableHeadersRow();

		std::int32_t id = 0;
		for (auto it = Log::BeginLogs(); it != Log::EndLogs(); ++it)
		{
			++id;
			std::string_view module = (*it)->GetModule();
			ELogVerbosity verbosities = (*it)->GetVerbosities();

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%s", module.data());

			for (size_t i = 1; i <= VerbosityLevels.size(); i++)
			{
				ImGui::PushID(id);
				ImGui::TableSetColumnIndex(i);

				auto &&[verb, _] = VerbosityLevels[i - 1];
				if (i == VerbosityLevels.size())
				{
					if (ImGui::Button("On"))
					{
						Log::SetVerbosities(module, ELogVerbosity::All);
					}
					ImGui::SameLine();
					if (ImGui::Button("Off"))
					{
						Log::SetVerbosities(module, ELogVerbosity::None);
					}
					ImGui::SameLine();
					if (ImGui::Button("Flip"))
					{
						ELogVerbosity flipped = ELogVerbosity::None;
						for (auto &&[v, _] : VerbosityLevels)
						{
							if (v != ELogVerbosity::All && v != ELogVerbosity::None && !EnumFlag::HasAny(verbosities, v))
							{
								EnumFlag::Mask(flipped, v);
							}
						}
						Log::SetVerbosities(module, flipped);
					}
				}
				else
				{
					bool enabled = EnumFlag::HasAny(verbosities, verb);
					if (ImGui::Checkbox("", &enabled))
					{
						Log::SetVerbosities(module, EnumFlag::BitXOr(verbosities, verb));
					}
				}

				ImGui::PopID();
				++id;
			}
		}

		ImGui::EndTable();
	}
}
