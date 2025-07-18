#pragma once

#include "Micros.hpp"
#include "Scopes.hpp"

namespace tudov
{
	template <ScopeEnum TEnum>
	TE_FORCEINLINE TEnum EnumFlagBitAnd(TEnum a, TEnum b) noexcept
	{
		return static_cast<TEnum>(static_cast<typename std::underlying_type<TEnum>::type>(a) & static_cast<typename std::underlying_type<TEnum>::type>(b));
	}

	template <ScopeEnum TEnum>
	TE_FORCEINLINE TEnum EnumFlagBitNot(TEnum a) noexcept
	{
		return static_cast<TEnum>(~static_cast<typename std::underlying_type<TEnum>::type>(a));
	}

	template <ScopeEnum TEnum>
	TE_FORCEINLINE TEnum EnumFlagBitOr(TEnum a, TEnum b) noexcept
	{
		return static_cast<TEnum>(static_cast<typename std::underlying_type<TEnum>::type>(a) | static_cast<typename std::underlying_type<TEnum>::type>(b));
	}

	template <ScopeEnum TEnum>
	TE_FORCEINLINE bool EnumFlagCheck(TEnum a, TEnum b) noexcept
	{
		return EnumFlagBitAnd(a, b) == b;
	}
} // namespace tudov