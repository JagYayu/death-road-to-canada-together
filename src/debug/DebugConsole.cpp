#include "DebugConsole.hpp"

#include <algorithm>
#include <imgui.h>
#include <tuple>

using namespace tudov;

DebugConsole::DebugConsole() noexcept
    : _log(Log::Get("DebugConsole"))
{
	_textBuffer = new ImGuiTextBuffer();
	_textFilter = new ImGuiTextFilter();

	_commands = {};

	auto &&help = [this](std::string_view)
	{
		std::vector<Result> results{};

		size_t minLen = 0;
		for (auto &&pair : _commands)
		{
			minLen = std::max(pair.second.name.size(), minLen);
		}
		for (auto &&[name, command] : _commands)
		{
			std::string str = name;
			str.append(minLen - str.size(), ' ');
			str.append(" - ");
			str.append(command.help);
			str.shrink_to_fit();
			results.emplace_back(Result{
			    .message = std::move(str),
			});
		}

		std::sort(results.begin(), results.end(), [](const Result &l, const Result &r)
		{
			return l.message < r.message;
		});

		results.insert(results.begin(), Result{
		                                    .message = "Listing all available commands",
		                                    .code = Code::Success,
		                                });

		return results;
	};
	_commands["help"] = Command{
	    .name = "help",
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
	// _textBuffer->clear();
	_lines.clear();
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
	if (begin == std::string_view::npos) [[unlikely]]
	{
		return;
	}

	Output("> {}\n", Code::None, command);

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
		Output("Unknown command \"{}\"", Code::Failure, command);
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
		        .message = std::format("[C++ exception] {}", e.what()),
		        .code = Code::Failure,
		    },
		};
	}

	for (auto &&result : results)
	{
		Output(result.message.data(), result.code);
	}
}

void DebugConsole::Output(std::string_view message, Code code)
{
	_lines.emplace_back(std::string(message), code);
}

void DebugConsole::UpdateAndRender(IWindow &window) noexcept
{
	static constexpr decltype(auto) colorFailure = ImVec4(1, 0, 0, 1);
	static constexpr decltype(auto) colorSuccess = ImVec4(.2, 1, .2, 1);
	static constexpr decltype(auto) colorWarn = ImVec4(1, 0, 0, 1);
	static constexpr decltype(auto) colorDefault = ImVec4(1, 1, 1, 1);

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
			for (auto &&[message, code] : _lines)
			{
				if (_textFilter->PassFilter(message.c_str(), message.c_str() + message.size()))
				{
					switch (code)
					{
					case Code::Failure:
					{
						ImGui::TextColored(colorFailure, "%s", message.c_str());
						break;
					}
					case Code::Success:
					{
						ImGui::TextColored(colorSuccess, "%s", message.c_str());
						break;
					}
					case Code::Warn:
					default:
					{
						ImGui::TextColored(colorDefault, "%s", message.c_str());
						break;
					}
					}
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
	}
	ImGui::End();
}