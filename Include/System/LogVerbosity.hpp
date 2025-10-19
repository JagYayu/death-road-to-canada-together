/**
 * @file system/LogVerbosity.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <cstdint>
#include <type_traits>

enum class ELogVerbosity : std::uint8_t
{
	All = static_cast<std::underlying_type_t<ELogVerbosity>>(-1),
	None = 0,
	Fatal = 1 << 0,
	Error = 1 << 1,
	Warn = 1 << 2,
	Info = 1 << 3,
	Debug = 1 << 4,
	Trace = 1 << 5,
	Default = Fatal | Error | Warn | Info,
};
