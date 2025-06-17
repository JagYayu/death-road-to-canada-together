#pragma once

#ifdef _MSC_VER

#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif

#else

#ifndef FORCEINLINE
#define FORCEINLINE inline
#endif

#endif

#include <type_traits>

#include <type_traits>

#define ENABLE_ENUM_FLAG_OPERATORS(E)                                 \
	inline E operator|(E a, E b)                                      \
	{                                                                 \
		using T = std::underlying_type_t<E>;                          \
		return static_cast<E>(static_cast<T>(a) | static_cast<T>(b)); \
	}                                                                 \
	inline E operator&(E a, E b)                                      \
	{                                                                 \
		using T = std::underlying_type_t<E>;                          \
		return static_cast<E>(static_cast<T>(a) & static_cast<T>(b)); \
	}                                                                 \
	inline E operator~(E a)                                           \
	{                                                                 \
		using T = std::underlying_type_t<E>;                          \
		return static_cast<E>(~static_cast<T>(a));                    \
	}                                                                 \
	inline E &operator|=(E &a, E b)                                   \
	{                                                                 \
		return a = a | b;                                             \
	}                                                                 \
	inline E &operator&=(E &a, E b)                                   \
	{                                                                 \
		return a = a & b;                                             \
	}
