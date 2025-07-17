#pragma once

#include <string>
namespace tudov
{
	struct IDebugManager;

	enum class DebugConsoleCode
	{
		None,
		Failure,
		Success,
		Warn,
	};

	struct DebugConsoleResult
	{
		std::string message;
		DebugConsoleCode code = DebugConsoleCode::None;
	};

	struct IDebugProvider
	{
		virtual ~IDebugProvider() noexcept = default;

		virtual void ProvideDebug(IDebugManager &debugManager) noexcept = 0;
	};
} // namespace tudov
