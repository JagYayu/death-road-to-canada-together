/**
 * @file util/LuaUtils.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Micros.hpp"

#include "sol/forward.hpp"
#include <sol/types.hpp>

namespace tudov
{
	struct IScriptEngine;

	struct LuaUtils
	{
		TE_STATIC_CLASS(LuaUtils);

		template <typename... TArgs>
		static void Deconstruct(TArgs &...objects) noexcept
		{
			DeconstructImpl(objects...);
		}

		static std::size_t GetArrayPartSize(sol::table tbl) noexcept;
		static std::size_t GetHashPartSize(sol::table tbl) noexcept;

		static std::size_t LuaGetArrayPartSize(sol::object tbl) noexcept;
		static std::size_t LuaGetHashPartSize(sol::object tbl) noexcept;

	  private:
		template <typename T, typename... TArgs>
		static void DeconstructImpl(T &first, TArgs &...rest) noexcept
		{
			using U = std::remove_reference_t<decltype(first)>;
			first.~U();
			DeconstructImpl(rest...);
		}

		static void DeconstructImpl() noexcept
		{
		}
	};
} // namespace tudov
