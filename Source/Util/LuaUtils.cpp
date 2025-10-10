/**
 * @file util/LuaUtils.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "util/LuaUtils.hpp"

#include "mod/ScriptEngine.hpp"
#include "system/LogMicros.hpp"

#include <cassert>
#include <cmath>
#include <format>
#include <sol/sol.hpp>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

using namespace tudov;

std::string DynamicFormat(IScriptEngine &scriptEngine, std::string_view fmt, sol::variadic_args vargs)
{
	return "";
}

std::size_t LuaUtils::GetArrayPartSize(sol::table tbl) noexcept
{
	TE_G_FATAL(TE_NAMEOF(LuaUtils), "NOT IMPLEMENTED");
}

std::size_t LuaUtils::GetHashPartSize(sol::table tbl) noexcept
{
	TE_G_FATAL(TE_NAMEOF(LuaUtils), "NOT IMPLEMENTED");
}

std::size_t LuaUtils::LuaGetArrayPartSize(sol::object tbl) noexcept
{
	TE_G_FATAL(TE_NAMEOF(LuaUtils), "NOT IMPLEMENTED");
}

std::size_t LuaUtils::LuaGetHashPartSize(sol::object tbl) noexcept
{
	TE_G_FATAL(TE_NAMEOF(LuaUtils), "NOT IMPLEMENTED");
}
