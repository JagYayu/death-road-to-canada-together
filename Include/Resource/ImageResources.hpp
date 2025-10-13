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
#include "Graphic/Image.hpp"
#include "Util/Definitions.hpp"

#include <string_view>

namespace tudov
{
	template <>
	inline Resources<Image>::Resources() noexcept
	    : _log(Log::Get("ImageResources"))
	{
	}

	class LuaBindings;
	class ScriptEngine;

	class ImageResources final : public Resources<Image>
	{
		friend LuaBindings;

	  public:
		explicit ImageResources() noexcept;
		explicit ImageResources(const ImageResources &) noexcept = delete;
		explicit ImageResources(ImageResources &&) noexcept = delete;
		ImageResources &operator=(const ImageResources &) noexcept = delete;
		ImageResources &operator=(ImageResources &&) noexcept = delete;
		~ImageResources() noexcept = default;

		void InstallToScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;
		void UninstallFromScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;

		ImageID LuaGetID(std::string_view image) noexcept;
		std::string_view LuaGetPath(ImageID id) noexcept;
	};
} // namespace tudov
