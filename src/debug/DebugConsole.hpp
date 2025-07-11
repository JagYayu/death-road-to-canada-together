#pragma once

#include "IDebugElement.hpp"
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
		enum class Code
		{
			None,
			Failure,
			Success,
			Warn,
		};

		struct Result
		{
			Code code;
			std::string message;
		};

		struct Command
		{
			std::string_view name;
			std::string help;
			std::function<std::vector<Result>(std::string_view)> func;
		};

	  private:
		std::shared_ptr<Log> _log;
		std::map<std::string, Command> _commands;

		ImGuiTextBuffer *_textBuffer;
		ImGuiTextFilter *_textFilter;
		std::vector<uint32_t> _lineOffsets;

	  public:
		static std::string_view Name() noexcept;

	  public:
		explicit DebugConsole() noexcept;
		~DebugConsole() noexcept;

	  public:
		std::string_view GetName() noexcept override;
		void UpdateAndRender(const std::shared_ptr<IWindow> &window) noexcept override;

		Command *GetCommand(std::string_view commandName) noexcept;
		void SetCommand(const Command &command) noexcept;
		void Execute(std::string_view command) noexcept;
		void Output(std::string_view message, Code code = Code::None);
		void Clear();

		const Command *GetCommand(std::string_view commandName) const noexcept
		{
			return const_cast<DebugConsole *>(this)->GetCommand(commandName);
		}

		template <typename... Args>
		inline void Output(std::format_string<Args...> fmt, Code code = Code::None, Args &&...args)
		{
			Output(std::format(fmt, std::forward<Args>(args)...), code);
		}
	};
} // namespace tudov
