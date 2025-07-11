#pragma once

namespace tudov
{
	struct IDebugManager;

	struct IDebugProvider
	{
		virtual ~IDebugProvider() noexcept = default;

		virtual void ProvideDebug(IDebugManager &debugManager) noexcept = 0;
	};
} // namespace tudov
