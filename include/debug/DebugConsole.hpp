#pragma once

#include "Debug.hpp"
#include "DebugElement.hpp"
#include "util/Log.hpp"

#include <functional>
#include <map>

struct ImGuiTextBuffer;
struct ImGuiTextFilter;

namespace tudov
{
	class DebugConsole : public IDebugElement
	{
	  public:
		using Code = DebugConsoleCode;
		using Result = DebugConsoleResult;

		struct Command
		{
			std::string name;
			std::string help;
			std::function<std::vector<Result>(std::string_view)> func;
		};

	  private:
		struct TextBufferSegment
		{
			const char *begin;
			const char *end;
		};

	  private:
		std::shared_ptr<Log> _log;
		std::map<std::string, Command> _commands;

		ImGuiTextBuffer *_textBuffer;
		ImGuiTextFilter *_textFilter;
		std::vector<std::tuple<std::string, Code>> _lines;

	  public:
		inline static constexpr std::string_view Name() noexcept
		{
			return "Console";
		}

	  public:
		explicit DebugConsole() noexcept;
		~DebugConsole() noexcept override;

	  public:
		std::string_view GetName() noexcept override;
		void UpdateAndRender(IWindow &window) noexcept override;

		Command *GetCommand(std::string_view commandName) noexcept;
		void SetCommand(const Command &command) noexcept;
		void Execute(std::string_view command) noexcept;
		void Output(std::string_view message, Code code = Code::None);
		void Clear();

		const Command *GetCommand(std::string_view commandName) const noexcept
		{
			return const_cast<DebugConsole *>(this)->GetCommand(commandName);
		}

		template <typename... TArgs>
		inline void Output(std::format_string<TArgs...> fmt, Code code = Code::None, TArgs &&...args)
		{
			Output(std::format(fmt, std::forward<TArgs>(args)...), code);
		}
	};
} // namespace tudov
