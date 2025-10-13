/**
 * @file Debug/Debug.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <string>

namespace tudov
{
	struct IDebugManager;

	enum class EDebugConsoleCode
	{
		None,
		Failure,
		Success,
		Warn,
	};

	struct DebugConsoleResult
	{
		std::string message;
		EDebugConsoleCode code = EDebugConsoleCode::None;
	};

	struct IDebugProvider
	{
		virtual ~IDebugProvider() noexcept = default;

		virtual void ProvideDebug(IDebugManager &debugManager) noexcept = 0;
	};
} // namespace tudov
