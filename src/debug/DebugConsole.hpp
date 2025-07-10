#pragma once

#include "IDebugElement.hpp"
#include "util/Log.hpp"

#include <imgui.h>

namespace tudov
{
	class DebugConsole : public IDebugElement
	{
	  public:
		struct Command
		{
		};

	  private:
		std::shared_ptr<Log> _log;

		ImGuiTextBuffer _buffer;
		ImGuiTextFilter _filter;
		std::vector<uint32_t> _lineOffsets;
		std::array<char, 256> _input;

	  public:
		explicit DebugConsole() noexcept;

		std::string_view GetName() noexcept override;
		void UpdateAndRender(const std::shared_ptr<IWindow> &window) noexcept override;

		void Output(std::string_view message);
		void Clear();

		template <typename... Args>
		inline void Output(std::format_string<Args...> fmt, Args &&...args)
		{
			Output(std::format(fmt, std::forward<Args>(args)...));
		}
	};
} // namespace tudov
