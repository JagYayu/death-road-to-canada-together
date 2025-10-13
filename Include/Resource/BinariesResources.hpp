/**
 * @file resource/BinariesResources.hpp
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
#include "misc/Binaries.hpp"

namespace tudov
{
	template <>
	inline Resources<Binaries>::Resources() noexcept
	    : _log(Log::Get("BinariesResources"))
	{
	}

	class LuaBindings;
	class ScriptEngine;

	class BinariesResources final : public Resources<Binaries>
	{
		friend LuaBindings;

	  public:
		explicit BinariesResources() noexcept;
		explicit BinariesResources(const BinariesResources &) noexcept = delete;
		explicit BinariesResources(BinariesResources &&) noexcept = delete;
		BinariesResources &operator=(const BinariesResources &) noexcept = delete;
		BinariesResources &operator=(BinariesResources &&) noexcept = delete;
		~BinariesResources() noexcept = default;
	};
} // namespace tudov
