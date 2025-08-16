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

	class LuaAPI;
	class ScriptEngine;

	class BinariesResources : public Resources<Binaries>
	{
		friend LuaAPI;

	  public:
		explicit BinariesResources() noexcept;
		~BinariesResources() noexcept = default;
	};
} // namespace tudov
