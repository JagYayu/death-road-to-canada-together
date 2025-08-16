/**
 * @file event/EventHandleKey.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "util/Scopes.hpp"

#include <cmath>
#include <string>
#include <type_traits>
#include <variant>

namespace tudov
{
	struct EventHandleKey
	{
		struct Equal
		{
			bool operator()(const EventHandleKey &lhs, const EventHandleKey &rhs) const
			{
				return lhs == rhs;
			}
		};

		struct Hash
		{
			std::size_t operator()(const EventHandleKey &key) const
			{
				if (std::holds_alternative<std::double_t>(key.value))
				{
					return std::hash<double>{}(std::get<std::double_t>(key.value));
				}
				else
				{
					return std::hash<std::string>{}(std::get<std::string>(key.value));
				}
			}
		};

		std::variant<std::nullptr_t, std::double_t, std::string> value;

		inline bool operator==(const EventHandleKey &other) const
		{
			return value == other.value;
		}

		inline bool IsAny() const noexcept
		{
			return std::holds_alternative<std::nullptr_t>(value);
		}

		inline bool Match(const EventHandleKey &key) const noexcept
		{
			return IsAny() || value == key.value;
		}

		inline EventHandleKey() noexcept
		    : value(nullptr)
		{
		}

		inline EventHandleKey(std::double_t number) noexcept
		    : value(number)
		{
		}

		inline explicit EventHandleKey(const std::string &string) noexcept
		    : value(string)
		{
		}

		inline explicit EventHandleKey(std::string_view stringView) noexcept
		    : value(std::string(stringView))
		{
		}

		inline EventHandleKey(const char *str) noexcept
		    : value(std::string(str))
		{
		}

		template <ScopeEnum TEnum>
		inline EventHandleKey(TEnum value) noexcept
		    : value(std::double_t(std::underlying_type_t<TEnum>(value)))
		{
		}

		~EventHandleKey() noexcept = default;
	};
} // namespace tudov
