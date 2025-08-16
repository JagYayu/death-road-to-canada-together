/**
 * @file network/SocketType.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <optional>
#include <string_view>

namespace tudov
{
	/**
	 * @brief Socket types
	 */
	enum class ESocketType
	{
		Local = 0,
		RUDP,
		TCP,
		Steam,
	};

	struct ISocketTypeProvider
	{
		virtual ESocketType GetSocketType() const noexcept = 0;
	};

	std::optional<ESocketType> StringToSocketType(std::string_view str) noexcept;
	// SocketTypeToString
} // namespace tudov
