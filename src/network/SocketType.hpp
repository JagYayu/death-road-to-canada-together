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
		UDP,
		Steam,
	};

	struct ISocketTypeProvider
	{
		virtual ESocketType GetSocketType() const noexcept = 0;
	};

	std::optional<ESocketType> StringToSocketType(std::string_view str) noexcept;
	// SocketTypeToString
} // namespace tudov
