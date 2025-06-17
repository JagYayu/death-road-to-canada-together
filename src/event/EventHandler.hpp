#pragma once

#include "sol/error.hpp"
#include "util/Defs.h"

#include <cmath>
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
				std::size_t operator()(const EventHandler::Key &key) const
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

			bool operator==(const Key &other) const
			{
				return value == other.value;
			}
		};

		struct Function
		{
			std::variant<sol::function, int> function;

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
						auto &&result = func();
						if (!result.valid())
						{
							sol::error err = result;
							throw err;
						}
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
				if (auto &&func = std::get_if<sol::function>(&function))
				{
					auto &&result = (*func)(obj, key);
					if (!result.valid())
					{
						sol::error err = result;
						throw err;
					}
				}
				else
				{
					// TODO
				}
			}
		};

		inline static constexpr std::double_t defaultSequence = 0;
		inline static const Key emptyKey = Key();

		EventID eventID;
		ScriptID scriptID;
		Function function;
		std::string name;
		std::string order;
		Key key;
		std::double_t sequence;
	};
} // namespace tudov