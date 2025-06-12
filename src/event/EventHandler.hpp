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
				bool operator()(const EventHandler::Key &lhs, const EventHandler::Key &rhs) const
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

		inline static Key emptyKey = Key(nullptr);
		inline static Number defaultSequence = 0;

		struct Function
		{
			Variant<sol::function, int> function;

			explicit Function(const sol::function& function)
			    : function(function)
			{
			}

			void operator()(const sol::object &obj) const
			{
				if (std::holds_alternative<sol::function>(function))
				{
					const auto &func = std::get<sol::function>(function);
					if (func.valid())
					{
						func();
					}
				}
				else
				{
					int i = std::get<int>(function);
					// TODO:
				}
			}

			void operator()(const sol::object &obj, const EventHandler::Key &key) const
			{
				if (auto &&func = GetIf<sol::function>(&function))
				{
					(*func)(obj, key);
				}
				else
				{
					// TODO
				}
			}
		};

		EventID eventID;
		ScriptID scriptID;
		Function function;
		String name;
		String order;
		Key key;
		Number sequence;
	};
} // namespace tudov