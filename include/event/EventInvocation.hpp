/**
 * @file event/EventInvocation.hpp
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

namespace tudov
{
	enum class EEventInvocation : std::uint8_t
	{
		All = static_cast<std::underlying_type_t<EEventInvocation>>(-1),
		None = 0,
		CacheHandlers = 1 << 0,
		NoProfiler = 1 << 1,
		TrackProgression = 1 << 2,

		Default = CacheHandlers,
	};
} // namespace tudov