/**
 * @file system/OperatingSystem.hpp
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

#include <cstdint>

namespace tudov
{
	enum class EOperatingSystem : std::uint8_t
	{
		Unknown = 0,
		Windows,
		Linux,
		MaxOS,
		Android,
		IOS,
	};

	struct OperatingSystem
	{
		TE_STATIC_CLASS(OperatingSystem);

		static constexpr EOperatingSystem GetType() noexcept
		{
#ifdef _WIN32
			return EOperatingSystem::Windows;
#else
			return EOperatingSystem::Unknown;
#endif
		}

		static constexpr bool IsMobile() noexcept
		{
			return false;
		}

		static constexpr bool IsPC() noexcept
		{
			return true;
		}
	};
} // namespace tudov
