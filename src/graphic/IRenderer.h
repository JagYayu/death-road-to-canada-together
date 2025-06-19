#pragma once

#include "IRenderBackend.h"

#include <memory>
#include <string_view>

namespace tudov
{
	class ScriptEngine;
	class Window;

	struct IRenderer : IRenderBackend
	{
		Window &window;

		IRenderer(Window &window) noexcept;

		virtual void Initialize() noexcept = 0;
		virtual void InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept = 0;

		virtual void Begin() = 0;
		virtual void End() = 0;
	};
} // namespace tudov
