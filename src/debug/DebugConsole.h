#pragma once

#include "IDebugElement.h"
#include "util/Defs.h"
#include "util/Log.h"

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
		SharedPtr<Log> _log;

		ImGuiTextBuffer _buffer;
		ImGuiTextFilter _filter;
		std::vector<uint32_t> _lineOffsets;
		std::array<Char, 256> _input;

	  public:
		explicit DebugConsole() noexcept;

		std::string_view GetName() noexcept override;
		void UpdateAndRender() noexcept override;

		void Log(std::string_view message);
		void Clear();
	};
} // namespace tudov
