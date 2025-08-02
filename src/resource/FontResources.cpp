#include "resource/FontResources.hpp"

using namespace tudov;

FontResources::FontResources() noexcept
{
}

// void FontResources::OnUnloadResource(ResourceID id) noexcept
// {
// 	_bitmapFonts.erase(id);
// }

// std::optional<std::reference_wrapper<BitmapFont>> FontResources::GetBitmapFont(TextureID textureID) noexcept
// {
// 	auto &&itInfo = _bitmapFonts.find(textureID);
// 	if (itInfo != _bitmapFonts.end())
// 	{
// 		return std::reference_wrapper(*itInfo->second);
// 	}
// 	return std::nullopt;
// }

// void FontResources::AddBitmapFont(const std::shared_ptr<BitmapFont> &bitmapFont) noexcept
// {
// 	_bitmapFonts[bitmapFont->GetTextureID()] = bitmapFont;
// }

void FontResources::InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept
{
}

void FontResources::UninstallFromScriptEngine(ScriptEngine &scriptEngine) noexcept
{
}
