#pragma once

#include <optional>
#include <string_view>

namespace tudov
{
	enum class ESocketType
	{
		Local = 0,
		ReliableUDP,
		Steam,
	};

	struct ISocketTypeProvider
	{
		virtual ESocketType GetSocketType() const noexcept = 0;
	};

	std::optional<ESocketType> StringToSocketType(std::string_view str) noexcept;

} // namespace tudov
