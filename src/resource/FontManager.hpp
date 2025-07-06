#pragma once

#include "ResourceManager.hpp"
#include "graphic/Font.h"

#include <string_view>

namespace tudov
{
	using FontManagerBase = ResourceManager<Font>;

	template <>
	inline FontManagerBase::ResourceManager() noexcept
	    : _log(Log::Get("FontManager"))
	{
	}

	class ScriptEngine;

	class FontManager : public FontManagerBase
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
		FontManager() noexcept;

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
