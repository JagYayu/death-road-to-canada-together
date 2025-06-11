#pragma once

#include "util/Defs.h"

#include "sol/sol.hpp"

namespace tudov
{
	struct EventHandler
	{
		struct Key
		{
			struct Equal
			{
				bool operator()(const tudov::EventHandler::Key &lhs, const tudov::EventHandler::Key &rhs) const
				{
					return lhs == rhs;
				}
			};

			struct Hash
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

			Variant<Nullptr, Number, String> value;

			bool operator==(const Key &other) const
			{
				return value == other.value;
			}
		};

		using KeyEqual = Key::Equal;
		using KeyHash = Key::Hash;

		inline static Key emptyKey = Key(nullptr);
		inline static Number defaultSequence = 0;

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
		String event;
		Function function;
		String name;
		String order;
		Key key;
		Number sequence;
	};
} // namespace tudov