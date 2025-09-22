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
#include "util/Micros.hpp"

#include <functional>
#include <stdexcept>
#include <variant>

namespace tudov
{
	struct EventHandleFunction
	{
		using Cpp = std::function<void(sol::object e, const EventHandleKey &key)>;
		using Lua = sol::protected_function;

		std::variant<Lua, Cpp> function;

		explicit EventHandleFunction(const Cpp &cppFunction)
		    : function(cppFunction)
		{
		}

		explicit EventHandleFunction(const Lua &luaFunction)
		    : function(luaFunction)
		{
		}

		TE_FORCEINLINE void operator()(sol::object obj) const
		{
			if (auto func = std::get_if<Lua>(&function); func != nullptr)
			{
				if (func->valid()) [[likely]]
				{
					auto result = (*func)(obj);
					if (!result.valid()) [[unlikely]]
					{
						sol::error err = result;
						throw err;
					}
				}
				else [[unlikely]]
				{
					throw std::runtime_error("Attempt to call invalid lua function");
				}
			}
			else if (auto func = std::get_if<Cpp>(&function); func != nullptr)
			{
				(*func)(obj, {});
			}
			else [[unlikely]]
			{
				throw std::runtime_error("Unknown function");
			}
		}

		TE_FORCEINLINE void operator()(sol::object obj, const EventHandleKey &key) const
		{
			if (auto func = std::get_if<Lua>(&function); func != nullptr)
			{
				auto result = (*func)(obj, key);
				if (!result.valid()) [[unlikely]]
				{
					sol::error err = result;
					throw err;
				}
			}
			else if (auto func = std::get_if<Cpp>(&function); func != nullptr)
			{
				(*func)(obj, key);
			}
			else [[unlikely]]
			{
				throw std::runtime_error("Unknown function");
			}
		}
	};
} // namespace tudov