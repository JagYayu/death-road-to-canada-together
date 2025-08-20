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

#include "debug/DebugLog.hpp"
#include "util/EnumFlag.hpp"

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

// todo 添加功能：
// todo 日志浏览器，打开日志目录，删除所有日志

void DebugLog::UpdateAndRender(IWindow &window) noexcept
{
	static auto TableLogFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg;
	static auto VerbosityLevels = std::vector<std::tuple<ELogVerbosity, std::string_view>>{
	    {ELogVerbosity::Trace, "Trace"},
	    {ELogVerbosity::Debug, "Debug"},
	    {ELogVerbosity::Info, "Info"},
	    {ELogVerbosity::Warn, "Warn"},
	    {ELogVerbosity::Error, "Error"},
	    {ELogVerbosity::All, "Toggle All"},
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
		for (auto &&it = Log::BeginLogs(); it != Log::EndLogs(); ++it)
		{
			++id;

			const std::string &module = it->first;
			ELogVerbosity verbosities = it->second->GetVerbosities();

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%s", it->first.c_str());

			for (size_t i = 1; i <= VerbosityLevels.size(); i++)
			{
				ImGui::PushID(id);
				ImGui::TableSetColumnIndex(i);

				auto &&[verb, str] = VerbosityLevels[i - 1];
				if (i == VerbosityLevels.size())
				{
					if (ImGui::Button("On"))
					{
						Log::SetVerbositiesOverride(module, ELogVerbosity::All);
					}
					ImGui::SameLine();
					if (ImGui::Button("Off"))
					{
						Log::SetVerbositiesOverride(module, ELogVerbosity::None);
					}
				}
				else
				{
					bool enabled = EnumFlag::HasAny(verbosities, verb);
					if (ImGui::Checkbox("", &enabled))
					{
						Log::SetVerbositiesOverride(module, EnumFlag::BitNot(verb));
					}
				}

				ImGui::PopID();
				++id;
			}
		}

		ImGui::EndTable();
	}
}
