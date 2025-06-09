#pragma once

#include "util/Defs.h"

#include "sol/sol.hpp"

namespace tudov
{
	struct EventHandler
	{
		struct Key
		{
			Variant<Number, String> value;

			bool operator==(const Key &other) const
			{
				return value == other.value;
			}
		};

		struct KeyEqual
		{
			bool operator()(const tudov::EventHandler::Key &lhs, const tudov::EventHandler::Key &rhs) const
			{
				return lhs == rhs;
			}
		};

		struct KeyHash
		{
			size_t operator()(const EventHandler::Key &key) const
			{
				if (Is<Number>(key.value))
				{
					return std::hash<double>{}(Get<Number>(key.value));
				}
				else
				{
					return std::hash<String>{}(Get<String>(key.value));
				}
			}
		};

		struct Function
		{
			Variant<sol::function, int> value;

			void operator()(const sol::object &obj) const
			{
				if (auto &&func = GetIf<sol::function>(&value))
				{
					(*func)(obj);
				}
				else
				{
					// TODO
				}
			}

			void operator()(const sol::object &obj, const EventHandler::Key &key) const
			{
				if (auto &&func = GetIf<sol::function>(&value))
				{
					(*func)(obj, key);
				}
				else
				{
					// TODO
				}
			}
		};

		String scriptName;
		String name;
		String order;
		Number sequence;
		Key key;
		Function function;
	};
} // namespace tudov