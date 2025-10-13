/**
 * @file program/CrashReporter.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Util/Micros.hpp"

namespace tudov
{
	struct CrashReporter
	{
		TE_STATIC_CLASS(CrashReporter);

		static void InitializeCrashReporter() noexcept;
	};
} // namespace tudov
