/**
 * @file system/RandomDevice.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "sol/forward.hpp"
#include "util/Micros.hpp"

#include <cmath>
#include <limits>

namespace tudov
{
	struct RandomDevice
	{
		TE_STATIC_CLASS(RandomDevice);

		static std::double_t Entropy() noexcept;
		static std::double_t Generate(std::double_t min = std::numeric_limits<std::double_t>::min(), std::double_t max = std::numeric_limits<std::double_t>::max()) noexcept;

		static std::double_t LuaGenerate(sol::object min, sol::object max) noexcept;
	};
} // namespace tudov
