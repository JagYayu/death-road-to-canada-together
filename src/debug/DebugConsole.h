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
		Vector<Int32> _lineOffsets;
		Array<Char, 256> _input;

	  public:
		explicit DebugConsole() noexcept;

		StringView GetName() noexcept override;
		void UpdateAndRender() noexcept override;

		void Log(StringView message);
		void Clear();
	};
} // namespace tudov
