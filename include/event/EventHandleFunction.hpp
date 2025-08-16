/**
 * @file event/EventHandlerFunction.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "EventHandleKey.hpp"

#include "sol/error.hpp"
#include "sol/forward.hpp"
#include "sol/function.hpp"

#include <variant>

namespace tudov
{
	struct EventHandleFunction
	{
		std::variant<sol::function, int> function;

		explicit EventHandleFunction(const sol::function &function)
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
					auto &&result = func(obj);
					if (!result.valid()) [[unlikely]]
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

		void operator()(const sol::object &obj, const EventHandleKey &key) const
		{
			if (auto &&func = std::get_if<sol::function>(&function); func != nullptr)
			{
				auto &&result = (*func)(obj, key);
				if (!result.valid()) [[unlikely]]
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
} // namespace tudov