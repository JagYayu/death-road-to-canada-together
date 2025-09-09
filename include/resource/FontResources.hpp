/**
 * @file resource/FontResources.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Resources.hpp"
#include "graphic/Font.hpp"

#include <string_view>

namespace tudov
{
	template <>
	inline Resources<Font>::Resources() noexcept
	    : _log(Log::Get("FontResources"))
	{
	}

	class ScriptEngine;

	class FontResources : public Resources<Font>
	{
	  public:
		FontResources() noexcept;

		FontID LuaGetID(std::string_view fontPath) noexcept;
		std::string_view LuaGetPath(FontID id) noexcept;
	};
} // namespace tudov
