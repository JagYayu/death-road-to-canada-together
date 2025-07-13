#pragma once

#ifdef _MSC_VER

#ifndef TUDOV_FORCEINLINE
#define TUDOV_FORCEINLINE __forceinline
#endif

#else

#ifndef TUDOV_FORCEINLINE
#define TUDOV_FORCEINLINE inline
#endif

#endif

#define TUDOV_ENUM_FLAG_OPERATORS(E)                                  \
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
