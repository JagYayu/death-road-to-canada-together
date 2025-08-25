/**
 * @file system/Time.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "util/Micros.hpp"

#include <cmath>

namespace tudov
{
	struct Time
	{
		TE_STATIC_CLASS(Time);

		static std::double_t GetSystemTime() noexcept;
	};
} // namespace tudov
