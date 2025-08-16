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
		//   public:
		// 	struct BitmapFontInfo
		// 	{
		// 		std::uint32_t charWidth;
		// 		std::uint32_t charHeight;
		// 		std::unordered_map<std::uint32_t, std::uint32_t> charMap;
		// 		std::uint32_t columns;
		// 	};

		//   private:
		// 	std::unordered_map<TextureID, std::shared_ptr<BitmapFont>> _bitmapFonts;

	  public:
		FontResources() noexcept;

	  protected:
		// void OnUnloadResource(ResourceID id) noexcept override;

	  public:
		// std::optional<std::reference_wrapper<BitmapFont>> GetBitmapFont(FontID fontID) noexcept;
		/*
		 * Override a texture resource as a bitmap.
		 */
		// void AddBitmapFont(const std::shared_ptr<BitmapFont> &bitmapFont) noexcept;

		void InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept;
		void UninstallFromScriptEngine(ScriptEngine &scriptEngine) noexcept;
	};
} // namespace tudov
