#include "FontManager.hpp"

using namespace tudov;

FontManager::FontManager() noexcept
{
}

std::optional<std::reference_wrapper<BitmapFont>> FontManager::GetBitmapFont(TextureID textureID) noexcept
{
	auto &&itInfo = _bitmapFonts.find(textureID);
	if (itInfo != _bitmapFonts.end())
	{
		return std::reference_wrapper(itInfo->second);
	}
	return std::nullopt;
}

void FontManager::SetBitmapFont(TextureID textureID, BitmapFont info) noexcept
{
	_bitmapFonts[textureID] = info;
}

void FontManager::InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept
{
}

void FontManager::UninstallFromScriptEngine(ScriptEngine &scriptEngine) noexcept
{
}
