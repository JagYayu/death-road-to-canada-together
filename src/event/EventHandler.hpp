#pragma once

#include "util/Defs.h"

#include <sol/sol.hpp>

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

		struct Function
		{
			Variant<sol::function, int> function;

			explicit Function(const sol::function &function)
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

			void operator()(const sol::object &obj, const Key &key) const
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

		inline static constexpr Number defaultSequence = 0;
		inline static const Key emptyKey = Key(nullptr);

		EventID eventID;
		ScriptID scriptID;
		Function function;
		String name;
		String order;
		Key key;
		Number sequence;
	};
} // namespace tudov