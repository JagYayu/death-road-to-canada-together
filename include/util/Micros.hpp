#pragma once

#ifdef _MSC_VER

#	ifndef TE_FORCEINLINE
#		define TE_FORCEINLINE __forceinline
#	endif

#else

#	ifndef TE_FORCEINLINE
#		define TE_FORCEINLINE inline
#	endif

#endif

/*
 * Parameter should typically be a class name, used to enable code navigation.
 * Note that some language services might not support this feature.
 */
#define TE_NAMEOF(Name) ("" #Name "")

#define TE_STATIC_CLASS(Class)                  \
	explicit Class() noexcept = delete; \
	~Class() noexcept = delete

#define TE_ENUM_FLAG_OPERATORS(E) static_assert(false, "Depreciated, use functions in `EnumFlag.hpp` to operate bits instead.")
// inline E operator|(E a, E b)                                      \
// {                                                                 \
// 	using T = std::underlying_type_t<E>;                          \
// 	return static_cast<E>(static_cast<T>(a) | static_cast<T>(b)); \
// }                                                                 \
// inline E operator&(E a, E b)                                      \
// {                                                                 \
// 	using T = std::underlying_type_t<E>;                          \
// 	return static_cast<E>(static_cast<T>(a) & static_cast<T>(b)); \
// }                                                                 \
// inline E operator~(E a)                                           \
// {                                                                 \
// 	using T = std::underlying_type_t<E>;                          \
// 	return static_cast<E>(~static_cast<T>(a));                    \
// }                                                                 \
// inline E &operator|=(E &a, E b)                                   \
// {                                                                 \
// 	return a = a | b;                                             \
// }                                                                 \
// inline E &operator&=(E &a, E b)                                   \
// {                                                                 \
// 	return a = a & b;                                             \
// }
