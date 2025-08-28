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
		std::variant<sol::protected_function, int> function;

		explicit EventHandleFunction(const sol::protected_function &function)
		    : function(function)
		{
		}

		void operator()(sol::object obj) const
		{
			if (std::holds_alternative<sol::protected_function>(function))
			{
				const auto &func = std::get<sol::protected_function>(function);
				if (func.valid())
				{
					sol::protected_function_result result = func(obj);
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

		void operator()(sol::object obj, const EventHandleKey &key) const
		{
			if (auto &&func = std::get_if<sol::protected_function>(&function); func != nullptr)
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