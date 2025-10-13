/**
 * @file util/Micros.hpp
 * @author JagYayu
 * @brief Micros for project header files.
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

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

#define TE_STATIC_CLASS(Class)          \
	explicit Class() noexcept = delete; \
	~Class() noexcept = delete

#define TE_DEFAULT_FIVE_RULES_CLASS(Class)              \
	~Class() noexcept = default;                        \
	explicit Class(const Class &) noexcept = default;   \
	explicit Class(Class &&) noexcept = default;        \
	Class &operator=(const Class &) noexcept = default; \
	Class &operator=(Class &&) noexcept = default

#define TE_DEFAULT_FIVE_RULES_VIRTUAL_CLASS(Class)      \
	explicit Class(const Class &) noexcept = default;   \
	explicit Class(Class &&) noexcept = default;        \
	Class &operator=(const Class &) noexcept = default; \
	Class &operator=(Class &&) noexcept = default
