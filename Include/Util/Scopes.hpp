/**
 * @file util/Scopes.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <map>
#include <string_view>
#include <type_traits>
#include <unordered_map>

namespace tudov
{
	template <typename T>
	concept ScopeEnum = std::is_enum_v<T> && !requires(T t) {
		{ t } -> std::convertible_to<std::underlying_type_t<T>>;
	};

	struct IDebugElement;

	template <typename T>
	concept ScopeIDebugElement = requires {
		std::is_base_of_v<IDebugElement, T>;
		{ T::Name() } -> std::convertible_to<std::string_view>;
	};

	template <typename T>
	concept ScopeMapOrUnorderedMap = requires {
		typename T::key_type;
		typename T::mapped_type;
	} && (std::is_same_v<T, std::map<typename T::key_type, typename T::mapped_type, typename T::key_compare, typename T::allocator_type>> || std::is_same_v<T, std::unordered_map<typename T::key_type, typename T::mapped_type, typename T::hasher, typename T::key_equal, typename T::allocator_type>>);
} // namespace tudov
