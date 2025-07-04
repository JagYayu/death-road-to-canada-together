#include "FontManager.hpp"

using namespace tudov;

FontManager::FontManager() noexcept
{
}

// void FontManager::OnUnloadResource(ResourceID id) noexcept
// {
// 	_bitmapFonts.erase(id);
// }

std::optional<std::reference_wrapper<BitmapFont>> FontManager::GetBitmapFont(TextureID textureID) noexcept
{
	auto &&itInfo = _bitmapFonts.find(textureID);
	if (itInfo != _bitmapFonts.end())
	{
		return std::reference_wrapper(*itInfo->second);
	}
	return std::nullopt;
}

void FontManager::AddBitmapFont(const std::shared_ptr<BitmapFont> &bitmapFont) noexcept
{
	_bitmapFonts[bitmapFont->GetTextureID()] = bitmapFont;
}

void FontManager::InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept
{
}

void FontManager::UninstallFromScriptEngine(ScriptEngine &scriptEngine) noexcept
{
}
