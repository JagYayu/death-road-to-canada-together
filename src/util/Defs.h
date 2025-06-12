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

	using Int32 = std::int32_t;
	using UInt32 = std::uint32_t;
	using UInt64 = std::uint64_t;
	using Nullptr = std::nullptr_t;
	using Number = double;
	using String = std::string;
	using StringView = std::string_view;
	using StringStream = std::stringstream;
	using OStringStream = std::ostringstream;
	using IStringStream = std::istringstream;

	inline constexpr auto &&null = std::nullopt;

	// containers

	template <typename T>
	using Vector = std::vector<T>;
	template <typename T>
	using List = std::list<T>;
	template <typename T>
	using Deque = std::deque<T>;
	template <typename T, std::size_t N>
	using Array = std::array<T, N>;
	template <typename T>
	using Set = std::set<T>;
	template <typename T, class Hash = std::hash<T>, class Equal = std::equal_to<T>>
	using UnorderedSet = std::unordered_set<T, Hash, Equal>;
	template <typename K, typename V>
	using Map = std::map<K, V>;
	template <typename K, typename V, typename Hash = std::hash<K>, typename Equal = std::equal_to<K>>
	using UnorderedMap = std::unordered_map<K, V, Hash, Equal>;
	template <typename T>
	using Stack = std::stack<T>;
	template <typename T>
	using Queue = std::queue<T>;
	template <typename T>
	using PriorityQueue = std::priority_queue<T>;
	template <typename T1, typename T2>
	using Pair = std::pair<T1, T2>;
	template <typename... Ts>
	using Tuple = std::tuple<Ts...>;
	template <typename... Types>
	using Variant = std::variant<Types...>;

	// pointers

	template <typename T>
	using SharedPtr = std::shared_ptr<T>;
	template <typename T>
	using UniquePtr = std::unique_ptr<T>;
	template <typename T>
	using WeakPtr = std::weak_ptr<T>;
	template <typename T>
	using Reference = std::reference_wrapper<T>;

	template <typename T, typename... Args>
	FORCEINLINE SharedPtr<T> MakeShared(Args &&...args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T, typename... Args>
	FORCEINLINE UniquePtr<T> MakeUnique(Args &&...args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

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

	// algorithm

	template <typename It>
	FORCEINLINE void Sort(It begin, It end)
	{
		std::sort(begin, end);
	}
	template <typename It, typename Compare>
	FORCEINLINE void Sort(It begin, It end, Compare comp)
	{
		std::sort(begin, end, comp);
	}
	template <typename It>
	FORCEINLINE void Reverse(It begin, It end)
	{
		std::reverse(begin, end);
	}
	template <typename It, typename T>
	FORCEINLINE It Find(It begin, It end, const T &value)
	{
		return std::find(begin, end, value);
	}
	template <typename It, typename Pred>
	FORCEINLINE It FindIf(It begin, It end, Pred p)
	{
		return std::find_if(begin, end, p);
	}
	template <typename It, typename Func>
	FORCEINLINE void ForEach(It begin, It end, Func f)
	{
		std::for_each(begin, end, f);
	}
	template <typename It, typename Pred>
	FORCEINLINE bool AnyOf(It begin, It end, Pred p)
	{
		return std::any_of(begin, end, p);
	}
	template <typename It, typename Pred>
	FORCEINLINE bool AllOf(It begin, It end, Pred p)
	{
		return std::all_of(begin, end, p);
	}
	template <typename It, typename Pred>
	FORCEINLINE bool NoneOf(It begin, It end, Pred p)
	{
		return std::none_of(begin, end, p);
	}
	template <typename InIt, typename OutIt>
	FORCEINLINE OutIt Copy(InIt begin, InIt end, OutIt dest)
	{
		return std::copy(begin, end, dest);
	}
	template <typename It, typename Pred>
	FORCEINLINE It RemoveIf(It begin, It end, Pred p)
	{
		return std::remove_if(begin, end, p);
	}
	template <typename Container, typename Predicate>
	FORCEINLINE void EraseIf(Container &container, Predicate p)
	{
		container.erase(RemoveIf(container.begin(), container.end(), p), container.end());
	}
	template <typename InIt, typename OutIt, typename Func>
	FORCEINLINE OutIt Transform(InIt begin, InIt end, OutIt dest, Func f)
	{
		return std::transform(begin, end, dest, f);
	}

	// utilities

	template <typename... Args>
	using Function = std::function<Args...>;
	template <typename... Ts>
	using Variant = std::variant<Ts...>;
	template <typename T>
	using Optional = std::optional<T>;

	template <typename T, typename... Ts>
	FORCEINLINE bool Is(const Variant<Ts...> &v)
	{
		return std::holds_alternative<T>(v);
	}
	template <typename T, typename... Ts>
	FORCEINLINE T &Get(Variant<Ts...> &v)
	{
		return std::get<T>(v);
	}
	template <typename T, typename... Ts>
	FORCEINLINE const T &Get(const Variant<Ts...> &v)
	{
		return std::get<T>(v);
	}
	template <typename T, typename... Ts>
	FORCEINLINE T *GetIf(Variant<Ts...> *v) noexcept
	{
		return std::get_if<T>(v);
	}
	template <typename T, typename... Ts>
	FORCEINLINE const T *GetIf(const Variant<Ts...> *v) noexcept
	{
		return std::get_if<T>(v);
	}
	template <typename... Ts, typename Visitor>
	FORCEINLINE decltype(auto) Visit(const Variant<Ts...> &v, Visitor &&visitor)
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
	FORCEINLINE String Format(std::format_string<Args...> fmt, Args &&...args)
	{
		return std::format(fmt, Forward<Args>(args)...);
	}

	template <typename... Args>
	FORCEINLINE void FormatTo(OStringStream &os, std::format_string<Args...> fmt, Args &&...args)
	{
		os << Format(fmt, Forward<Args>(args)...);
	}
	template <typename... Args>
	FORCEINLINE void FormatTo(OStringStream &os, std::locale loc, std::format_string<Args...> fmt, Args &&...args)
	{
		os << Format(loc, fmt, Forward<Args>(args)...);
	}

	// engine types

	using EventID = UInt64;
	using ScriptID = UInt64;
} // namespace tudov