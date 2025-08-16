/**
 * @file resource/TextureResources.hpp
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
#include "graphic/Texture.hpp"

#include <string_view>

namespace tudov
{
	template <>
	inline Resources<Texture>::Resources() noexcept
	    : _log(Log::Get("TextureResources"))
	{
	}

	class ScriptEngine;

	class TextureResources : public Resources<Texture>
	{
	  public:
		explicit TextureResources() noexcept;
		~TextureResources() noexcept = default;

		void InstallToScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;
		void UninstallFromScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;
	};
} // namespace tudov
