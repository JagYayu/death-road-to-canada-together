/**
 * @file resource/ImageResources.hpp
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
#include "graphic/Image.hpp"
#include "util/Definitions.hpp"

#include <string_view>

namespace tudov
{
	template <>
	inline Resources<Image>::Resources() noexcept
	    : _log(Log::Get("ImageResources"))
	{
	}

	class LuaAPI;
	class ScriptEngine;

	class ImageResources : public Resources<Image>
	{
		friend LuaAPI;

	  public:
		explicit ImageResources() noexcept;

		void InstallToScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;
		void UninstallFromScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;

		ImageID LuaGetID(const std::string_view &image) noexcept;
		std::string_view LuaGetPath(ImageID id) noexcept;
	};
} // namespace tudov
