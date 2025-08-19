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
#include <sol/types.hpp>

namespace tudov
{
	struct LuaUtils
	{
		TE_STATIC_CLASS(LuaUtils);

		template <typename... TArgs>
		static void Deconstruct(TArgs &...objects) noexcept
		{
			DeconstructImpl(objects...);
		}

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
