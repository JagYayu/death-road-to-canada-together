#pragma once

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
} // namespace tudov
