#pragma once

#include "Micros.h"

#include <array>
#include <cstddef>
#include <deque>
#include <format>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace tudov
{
	// basic types

	using Char = char;
	using Int64 = std::int64_t;
	using UInt32 = std::uint32_t;
	using UInt64 = std::uint64_t;
	using Nullptr = std::nullptr_t;
	using Number = double;

	inline constexpr auto &&nullopt = std::nullopt;

	// pointers

	template <typename T>
	using SharedPtr = std::shared_ptr<T>;
	template <typename T>
	using UniquePtr = std::unique_ptr<T>;
	template <typename T>
	using WeakPtr = std::weak_ptr<T>;
	template <typename T>
	using Reference = std::reference_wrapper<T>;

	template <typename T>
	FORCEINLINE Reference<T> Ref(T &t)
	{
		return std::ref(t);
	}
	template <typename T>
	FORCEINLINE Reference<const T> CRef(const T &t)
	{
		return std::cref(t);
	}

	// utilities

	template <typename T, typename... Ts>
	FORCEINLINE bool Is(const std::variant<Ts...> &v)
	{
		return std::holds_alternative<T>(v);
	}
	template <typename T, typename... Ts>
	FORCEINLINE T &Get(std::variant<Ts...> &v)
	{
		return std::get<T>(v);
	}
	template <typename T, typename... Ts>
	FORCEINLINE const T &Get(const std::variant<Ts...> &v)
	{
		return std::get<T>(v);
	}
	template <typename T, typename... Ts>
	FORCEINLINE T *GetIf(std::variant<Ts...> *v) noexcept
	{
		return std::get_if<T>(v);
	}
	template <typename T, typename... Ts>
	FORCEINLINE const T *GetIf(const std::variant<Ts...> *v) noexcept
	{
		return std::get_if<T>(v);
	}
	template <typename... Ts, typename Visitor>
	FORCEINLINE decltype(auto) Visit(const std::variant<Ts...> &v, Visitor &&visitor)
	{
		return std::visit(std::forward<Visitor>(visitor), v);
	}

	template <typename T>
	FORCEINLINE constexpr std::remove_reference_t<T> &&Move(T &&t) noexcept
	{
		return std::move(t);
	}
	template <typename T>
	FORCEINLINE constexpr T &&Forward(std::remove_reference_t<T> &t) noexcept
	{
		return std::forward<T>(t);
	}
	template <typename T>
	FORCEINLINE constexpr T &&Forward(std::remove_reference_t<T> &&t) noexcept
	{
		static_assert(!std::is_lvalue_reference<T>::value, "Bad forward of rvalue as lvalue.");
		return std::forward<T>(t);
	}

	template <typename... Args>
	FORCEINLINE std::string Format(std::format_string<Args...> fmt, Args &&...args)
	{
		return std::format(fmt, Forward<Args>(args)...);
	}

	// engine types

	using EventID = UInt64;
	using ResourceID = UInt64;
	using ScriptID = UInt64;
} // namespace tudov