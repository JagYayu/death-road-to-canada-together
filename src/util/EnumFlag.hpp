#pragma once

#include "Micros.hpp"
#include "Scopes.hpp"

namespace tudov
{
	struct EnumFlag
	{
		explicit EnumFlag() noexcept = delete;
		~EnumFlag() noexcept = delete;

		template <ScopeEnum TEnum>
		TE_FORCEINLINE static constexpr TEnum BitAnd(TEnum a, TEnum b) noexcept
		{
			return static_cast<TEnum>(static_cast<typename std::underlying_type<TEnum>::type>(a) & static_cast<typename std::underlying_type<TEnum>::type>(b));
		}

		template <ScopeEnum TEnum>
		TE_FORCEINLINE static constexpr TEnum BitNot(TEnum a) noexcept
		{
			return static_cast<TEnum>(~static_cast<typename std::underlying_type<TEnum>::type>(a));
		}

		template <ScopeEnum TEnum>
		TE_FORCEINLINE static constexpr TEnum BitOr(TEnum a, TEnum b) noexcept
		{
			return static_cast<TEnum>(static_cast<typename std::underlying_type<TEnum>::type>(a) | static_cast<typename std::underlying_type<TEnum>::type>(b));
		}

		template <ScopeEnum TEnum>
		TE_FORCEINLINE static constexpr TEnum BitXOr(TEnum a, TEnum b) noexcept
		{
			return static_cast<TEnum>(static_cast<std::underlying_type_t<TEnum>>(a) ^ static_cast<std::underlying_type_t<TEnum>>(b));
		}

		template <ScopeEnum TEnum>
		TE_FORCEINLINE static constexpr bool HasAll(TEnum a, TEnum b) noexcept
		{
			return BitAnd(a, b) == b;
		}

		template <ScopeEnum TEnum>
		TE_FORCEINLINE static constexpr bool HasAny(TEnum a, TEnum b) noexcept
		{
			return BitAnd(a, b) != TEnum(0);
		}

		template <ScopeEnum TEnum>
		TE_FORCEINLINE static constexpr void Mask(TEnum &a, TEnum b) noexcept
		{
			a = BitOr(a, b);
		}

		template <ScopeEnum TEnum>
		TE_FORCEINLINE static constexpr void Toggle(TEnum &a, TEnum b) noexcept
		{
			a = BitXOr(a, b);
		}

		template <ScopeEnum TEnum>
		TE_FORCEINLINE static constexpr void Unmask(TEnum &a, TEnum b) noexcept
		{
			a = BitAnd(a, BitNot(b));
		}
	};
} // namespace tudov