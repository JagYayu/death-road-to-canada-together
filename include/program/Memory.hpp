/**
 * @file program/Memory.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <cstddef>

namespace tudov
{
	class Memory
	{
		static std::size_t GetHeapUsage() noexcept;
	};
}