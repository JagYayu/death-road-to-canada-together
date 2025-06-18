#pragma once

#include <cmath>
#include <string>
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

		bool operator==(const EventHandleKey &other) const
		{
			return value == other.value;
		}
	};
} // namespace tudov
