// DEPRECIATED LIBRARY DO NOT USE

/*
 * @file    InternString.hpp
 * @brief   Thread-safe string interning library for unified management and reuse of identical strings
 * @author  JagYayu
 * @version 1.0
 * @license WTFPL
 * @git     https://github.com/JagYayu/InternString
 * @details
 * This library provides a templated InternString class:
 *  - Uses `std::shared_ptr` and `std::weak_ptr` to ensure a single instance per unique string
 *  - Employs shared mutex for high-concurrency read/write access
 *  - Supports customizable character types (char, wchar_t)
 * Primarily intended for long-lived strings (e.g., configuration keys, identifiers), not suitable for short-lived or temporary strings.
 * Should periodically call `Clean()` to remove strings that have been released.
 */

#pragma once

#include <memory>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>

namespace intern_string
{
	template <typename TElement, class TTraits = std::char_traits<TElement>, class TAllocator = std::allocator<TElement>>
	class BasicInternString
	{
		using TString = std::basic_string<TElement, TTraits, TAllocator>;
		using TStringView = std::basic_string_view<TElement, TTraits>;

		using THash = std::hash<TStringView>;
		using TSharedMutex = std::shared_mutex;

	  private:
		struct KeyHash
		{
			using is_transparent = void;

			inline size_t operator()(TStringView sv) const noexcept
			{
				return THash{}(sv);
			}

			inline size_t operator()(const TString &s) const noexcept
			{
				return THash{}(s);
			}
		};

		struct KeyEqual
		{
			using is_transparent = void;

			inline bool operator()(TStringView a, TStringView b) const noexcept
			{
				return a == b;
			}

			inline bool operator()(TStringView sv, const TString &s) const noexcept
			{
				return sv == TStringView(s);
			}

			inline bool operator()(const TString &s, TStringView sv) const noexcept
			{
				return TStringView(s) == sv;
			}

			bool operator()(const TString &lhs, const TString &rhs) const noexcept
			{
				return TStringView(lhs) == TStringView(rhs);
			}
		};

		class Pool
		{
			std::unordered_map<TString, std::weak_ptr<TString>, KeyHash, KeyEqual> _map;
			mutable TSharedMutex _mtx;

		  public:
			inline std::shared_ptr<TString> Intern(TStringView sv)
			{
				{
					std::shared_lock lock(_mtx);
					auto it = _map.find(sv);
					if (it != _map.end())
					{
						if (auto sp = it->second.lock())
						{
							return sp;
						}
					}
				}

				std::unique_lock lock(_mtx);

				auto it = _map.find(sv);
				if (it != _map.end())
				{
					if (auto sp = it->second.lock())
					{
						return sp;
					}

					_map.erase(it);
				}

				auto sp = std::make_shared<TString>(sv);

				_map.emplace(*sp, std::weak_ptr<TString>(sp));
				return sp;
			}

			inline void Clean()
			{
				std::unique_lock lock(_mtx);
				for (auto it = _map.begin(); it != _map.end();)
				{
					if (it->second.expired())
					{
						it = _map.erase(it);
					}
					else
					{
						++it;
					}
				}
			}

			size_t size() const noexcept
			{
				std::shared_lock lock(_mtx);
				return _map.size();
			}
		};

	  public:
		struct Hash
		{
			inline size_t operator()(const BasicInternString &s) const noexcept
			{
				return THash{}(TStringView(s));
			}
		};

	  private:
		inline static Pool _pool;

		std::shared_ptr<TString> _ptr;

	  public:
		inline static const TString empty = TString("");
		inline static const TStringView emptyView = TStringView("");

		static Pool &GetPool()
		{
			return _pool;
		}

		BasicInternString() = default;
		BasicInternString(TStringView strView)
		    : _ptr(_pool.Intern(strView))
		{
		}
		BasicInternString(const TString &str)
		    : _ptr(_pool.Intern(str))
		{
		}
		BasicInternString(TString &&str)
		    : _ptr(_pool.Intern(str))
		{
		}

		BasicInternString(const BasicInternString &) = default;
		BasicInternString(BasicInternString &&) noexcept = default;
		BasicInternString &operator=(const BasicInternString &) = default;
		BasicInternString &operator=(BasicInternString &&) noexcept = default;

		inline size_t GetRefCount() const noexcept
		{
			return _ptr ? _ptr.use_count() : 0;
		}

		inline TString String() const noexcept
		{
			return _ptr ? *_ptr : empty;
		}

		inline TStringView StringView() const noexcept
		{
			return _ptr ? *_ptr : emptyView;
		}

		inline explicit operator TString() const noexcept
		{
			return _ptr ? *_ptr : empty;
		}
		inline operator TStringView() const noexcept
		{
			return _ptr ? *_ptr : emptyView;
		}

		friend inline bool operator==(const BasicInternString &a, const BasicInternString &b) noexcept
		{
			return TStringView(a) == TStringView(b);
		}
		friend inline bool operator!=(const BasicInternString &a, const BasicInternString &b) noexcept
		{
			return TStringView(a) != TStringView(b);
		}
		friend inline bool operator<(const BasicInternString<char> &a, const BasicInternString<char> &b)
		{
			return TStringView(a) < TStringView(b);
		}
	};

	using InternString = BasicInternString<char>;
	using InternWString = BasicInternString<wchar_t>;
#ifdef __cpp_lib_char8_t
	using InternU8String = BasicInternString<char8_t>;
#endif
	using InternU16String = BasicInternString<char16_t>;
	using InternU32String = BasicInternString<char32_t>;
} // namespace intern_string

namespace std
{
	template <typename TElement, class TTraits, class TAllocator>
	struct hash<intern_string::BasicInternString<TElement, TTraits, TAllocator>>
	{
		size_t operator()(const intern_string::BasicInternString<TElement, TTraits, TAllocator> &s) const noexcept
		{
			return std::hash<std::basic_string_view<TElement, TTraits>>{}(static_cast<std::basic_string_view<TElement, TTraits>>(s));
		}
	};
} // namespace std

#include "json.hpp"

namespace intern_string
{
	template <typename TElement, class TTraits, class TAllocator>
	struct BasicInternStringJsonSerializer
	{
		static void to_json(nlohmann::json &j, const BasicInternString<TElement, TTraits, TAllocator> &s)
		{
			j = static_cast<std::basic_string<TElement, TTraits, TAllocator>>(s);
		}

		static void from_json(const nlohmann::json &j, BasicInternString<TElement, TTraits, TAllocator> &s)
		{
			if (j.is_null())
			{
				s = BasicInternString<TElement, TTraits, TAllocator>();
			}
			else
			{
				s = BasicInternString<TElement, TTraits, TAllocator>(j.get<std::basic_string<TElement, TTraits, TAllocator>>());
			}
		}
	};
} // namespace intern_string

namespace nlohmann
{
	// Specialize adl_serializer for all BasicInternString types
	template <typename TElement, class TTraits, class TAllocator>
	struct adl_serializer<intern_string::BasicInternString<TElement, TTraits, TAllocator>>
	    : public intern_string::BasicInternStringJsonSerializer<TElement, TTraits, TAllocator>
	{
	};
} // namespace nlohmann
