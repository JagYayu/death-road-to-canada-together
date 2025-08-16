/**
 * @file resource/TextResources.hpp
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
#include "misc/Text.hpp"

#include <string_view>

namespace tudov
{
	template <>
	inline Resources<Text>::Resources() noexcept
	    : _log(Log::Get("TextResources"))
	{
	}

	class LuaAPI;
	class ScriptEngine;

	class TextResources : public Resources<Text>
	{
		friend LuaAPI;

	  public:
		explicit TextResources() noexcept = default;
		~TextResources() override = default;
	};
} // namespace tudov
