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

	class LuaBindings;
	class ScriptEngine;

	class TextResources final : public Resources<Text>
	{
		friend LuaBindings;

	  public:
		explicit TextResources() noexcept = default;
		explicit TextResources(const TextResources &) noexcept = delete;
		explicit TextResources(TextResources &&) noexcept = delete;
		TextResources &operator=(const TextResources &) noexcept = delete;
		TextResources &operator=(TextResources &&) noexcept = delete;
		~TextResources() override = default;
	};
} // namespace tudov
