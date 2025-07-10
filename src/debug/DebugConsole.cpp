#include "DebugConsole.hpp"

#include "event/EventManager.hpp"

#include <imgui.h>

using namespace tudov;

DebugConsole::DebugConsole() noexcept
    : _log(Log::Get("DebugConsole")),
      _input()
{
}

std::string_view DebugConsole::GetName() noexcept
{
	return "Console";
}

void DebugConsole::Clear()
{
	_buffer.clear();
	_lineOffsets.clear();
	_lineOffsets.push_back(0);
}

void DebugConsole::Output(std::string_view message)
{
	int old_size = _buffer.size();
	_buffer.append(message.data());

	for (int new_size = _buffer.size(); old_size < new_size; old_size++)
	{
		if (_buffer[old_size] == '\n')
		{
			_lineOffsets.push_back(old_size + 1);
		}
	}
}

static bool my_tool_active = false;
static float my_color = -1;

void DebugConsole::UpdateAndRender(const std::shared_ptr<IWindow> &window) noexcept
{
	if (ImGui::Begin("Debug Console"))
	{
		if (ImGui::Button("Clear"))
		{
			Clear();
		}
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		_filter.Draw("Filter");

		ImGui::Separator();

		{
			ImGui::BeginChild("ScrollRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);

			if (copy)
			{
				ImGui::LogToClipboard();
			}

			const char *buf = _buffer.begin();
			for (int line_no = 0; line_no < _lineOffsets.size(); line_no++)
			{
				const char *line_start = buf + _lineOffsets[line_no];
				const char *line_end = (line_no + 1 < _lineOffsets.size()) ? buf + _lineOffsets[line_no + 1] - 1 : _buffer.end();
				if (_filter.PassFilter(line_start, line_end))
				{
					ImGui::TextUnformatted(line_start, line_end);
				}
			}

			if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			{
				ImGui::SetScrollHereY(1);
			}

			ImGui::EndChild();
		}

		ImGui::Separator();

		ImGui::Text(">");
		ImGui::SameLine();
		if (ImGui::InputText("Input", _input.data(), _input.size(), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			if (_input[0])
			{
				Output("> {}\n", _input.data());
				// window->GetEventManager()->GetCoreEvents().KeyDown()
				// TODO: 处理命令
				_input[0] = 0;
			}
		}
	}
	ImGui::End();
}