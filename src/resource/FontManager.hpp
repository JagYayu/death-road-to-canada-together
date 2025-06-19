#pragma once

#include "ResourceManager.hpp"
#include "graphic/BitmapFont.h"
#include "graphic/IFont.h"
#include "util/Defs.h"

#include <functional>
#include <optional>
#include <string_view>
#include <unordered_map>

namespace tudov
{
	using FontManagerBase = ResourceManager<IFont>;

	template <>
	inline FontManagerBase::ResourceManager() noexcept
	    : _log(Log::Get("TextureManager"))
	{
	}

	class ScriptEngine;

	class FontManager : public FontManagerBase
	{
	  public:
		struct BitmapFontInfo
		{
			std::uint32_t charWidth;
			std::uint32_t charHeight;
			std::unordered_map<std::uint32_t, std::uint32_t> charMap;
			std::uint32_t columns;
		};

	  private:
		std::unordered_map<TextureID, BitmapFont> _bitmapFonts;

	  public:
		FontManager() noexcept;

		std::optional<std::reference_wrapper<BitmapFont>> GetBitmapFont(FontID fontID) noexcept;
		/*
		 * Override a texture resource as a bitmap.
		 */
		void SetBitmapFont(TextureID textureID, BitmapFont info) noexcept;

		void InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept;
		void UninstallFromScriptEngine(ScriptEngine &scriptEngine) noexcept;
	};
} // namespace tudov
