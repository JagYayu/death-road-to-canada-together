/**
 * @file Mod/ModPortal.hpp
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

namespace tudov
{
	enum class EModPortal : std::uint8_t
	{
		ModIO,
	};

	struct IModPortal
	{
		virtual EModPortal GetModPortalType() const noexcept = 0;
	};
} // namespace tudov
