/**
 * @file graphic/TextureManager.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "graphic/TextureManager.hpp"

#include <memory>
#include <tuple>

using namespace tudov;

TextureManager::TextureManager() noexcept
{
}

std::shared_ptr<Texture> TextureManager::GetTexture(TextureID textureID) noexcept
{
	auto it = _textures.find(textureID);
	if (it == _textures.end()) [[unlikely]]
	{
		return nullptr;
	}

	return it->second;
}

std::tuple<std::shared_ptr<Texture>, TextureID> TextureManager::LoadTexture(Renderer &renderer) noexcept
{
	auto texture = std::make_shared<Texture>(renderer);

	++_latestID;
	_textures[_latestID] = texture;

	return {texture, _latestID};
}
