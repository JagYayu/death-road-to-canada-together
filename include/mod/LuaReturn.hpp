/**
 * @file mod/LuaException.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "sol/forward.hpp"
#include "util/Micros.hpp"

#include "sol/object.hpp"
#include "sol/string_view.hpp"

#include <string>
#include <variant>

namespace tudov
{
	class LuaReturn
	{
	  private:
		std::variant<sol::object, std::string> _value;

	  public:
		static LuaReturn Error(std::string_view message) noexcept
		{
			LuaReturn luaReturn;
			luaReturn._value = {std::string(message)};
			return luaReturn;
		}

	  private:
		explicit LuaReturn() noexcept
		{
		}

	  public:
		LuaReturn(sol::object obj) noexcept
		    : _value({obj})
		{
		}

		TE_FORCEINLINE bool HasError() noexcept
		{
			return std::holds_alternative<std::string>(_value);
		}

		TE_FORCEINLINE sol::string_view GetError() noexcept
		{
			if (std::holds_alternative<std::string>(_value)) [[likely]]
			{
				return std::get<std::string>(_value);
			}
			else [[unlikely]]
			{
				return "";
			}
		}

		TE_FORCEINLINE sol::object GetObject() noexcept
		{
			if (std::holds_alternative<sol::object>(_value)) [[likely]]
			{
				return std::get<sol::object>(_value);
			}
			else [[unlikely]]
			{
				return {};
			}
		}
	};
} // namespace tudov
