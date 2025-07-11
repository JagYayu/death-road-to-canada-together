#include "DebugConsole.hpp"

#include <algorithm>
#include <imgui.h>

using namespace tudov;

DebugConsole::DebugConsole() noexcept
    : _log(Log::Get("DebugConsole"))
{
	_textBuffer = new ImGuiTextBuffer();
	_textFilter = new ImGuiTextFilter();

	_commands = {};

	const auto name = "help";
	auto &&help = [this](std::string_view)
	{
		std::vector<Result> results{};
		return results;
	};
	_commands[name] = Command{
	    .name = name,
	    .help = "help [regex] [max_cmds]: List `help` of all available/matched commands.",
	    .func = help,
	};
}

DebugConsole::~DebugConsole() noexcept
{
	delete _textBuffer;
	delete _textFilter;
}

std::string_view DebugConsole::Name() noexcept
{
	return "Console";
}

std::string_view DebugConsole::GetName() noexcept
{
	return Name();
}

void DebugConsole::Clear()
{
	_textBuffer->clear();
	_lineOffsets.clear();
	_lineOffsets.push_back(0);
}

DebugConsole::Command *DebugConsole::GetCommand(std::string_view commandName) noexcept
{
	auto &&it = _commands.find(std::string(commandName));
	return it == _commands.end() ? nullptr : &it->second;
}

void DebugConsole::SetCommand(const Command &command) noexcept
{
	auto &&commandName = std::string(command.name);
	Command commandClone = command;
	commandClone.name = commandName;
	_commands[commandName] = commandClone;
}

void DebugConsole::Execute(std::string_view command) noexcept
{
	auto begin = command.find_first_not_of(" \t");
	if (begin == std::string_view::npos)
	{
		return;
	}

	auto end = command.find_first_of(" \t", begin);
	auto commandName = command.substr(begin, end - begin);

	std::string_view args;
	if (end != std::string_view::npos)
	{
		args = command.substr(end + 1);
		if (auto args_start = args.find_first_not_of(" \t"); args_start != std::string_view::npos)
		{
			args = args.substr(args_start);
		}
		else
		{
			args = "";
		}
	}

	auto &&it = std::find_if(_commands.begin(), _commands.end(), [&](const std::pair<std::string, Command> &pair)
	{
		return pair.second.name == commandName;
	});

	if (it == _commands.end())
	{
		// Command not found - you might want to handle this case
		// e.g., OutputMessage("Unknown command");
		return;
	}

	std::vector<Result> results;
	std::string_view message;
	try
	{
		results = it->second.func(args);
	}
	catch (std::exception &e)
	{
		results = {
		    {
		        .code = Code::Failure,
		        .message = e.what(),
		    },
		};
	}

	Output("> {}\n", Code::None, command);
	for (auto &&result : results)
	{
		Output(result.message.data(), result.code);
	}
}

void DebugConsole::Output(std::string_view message, Code code)
{
	auto old_size = _textBuffer->size();
	_textBuffer->append(message.data());

	for (auto new_size = _textBuffer->size(); old_size < new_size; old_size++)
	{
		if ((*_textBuffer)[old_size] == '\n')
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
		_textFilter->Draw("Filter");

		ImGui::Separator();

		{
			ImGui::BeginChild("ScrollRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);

			if (copy)
			{
				ImGui::LogToClipboard();
			}

			const char *buf = _textBuffer->begin();
			for (int line_no = 0; line_no < _lineOffsets.size(); line_no++)
			{
				auto &&line_start = buf + _lineOffsets[line_no];
				auto &&line_end = (line_no + 1 < _lineOffsets.size()) ? buf + _lineOffsets[line_no + 1] - 1 : _textBuffer->end();
				if (_textFilter->PassFilter(line_start, line_end))
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
		std::array<char, 256> input{};
		if (ImGui::InputText("Input", input.data(), input.size(), ImGuiInputTextFlags_EnterReturnsTrue) && input[0])
		{
			Execute(input.data());
		}

		// TODO: 处理命令
		input[0] = 0;
	}
	ImGui::End();
}