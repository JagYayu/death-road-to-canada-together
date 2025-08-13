#include "debug/DebugLog.hpp"
#include "util/EnumFlag.hpp"

#include "imgui.h"

#include <tuple>
#include <unordered_map>
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
	static auto VerbosityLevels = std::vector<std::tuple<Log::EVerbosity, std::string_view>>{
	    {Log::EVerbosity::Trace, "Trace"},
	    {Log::EVerbosity::Debug, "Debug"},
	    {Log::EVerbosity::Info, "Info"},
	    {Log::EVerbosity::Warn, "Warn"},
	    {Log::EVerbosity::Error, "Error"},
	    {Log::EVerbosity::All, "Toggle All"},
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
			Log::EVerbosity verbosities = it->second->GetVerbosity();

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
						Log::SetVerbosity(module, Log::EVerbosity::All);
					}
					ImGui::SameLine();
					if (ImGui::Button("Off"))
					{
						Log::SetVerbosity(module, Log::EVerbosity::None);
					}
				}
				else
				{
					bool enabled = EnumFlag::HasAny(verbosities, verb);
					if (ImGui::Checkbox("", &enabled))
					{
						Log::SetVerbosity(module, EnumFlag::BitNot(verb));
					}
				}

				ImGui::PopID();
				++id;
			}
		}

		ImGui::EndTable();
	}
}
