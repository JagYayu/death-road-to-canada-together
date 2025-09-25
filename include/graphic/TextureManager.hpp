/**
 * @file graphic/TextureManager.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "graphic/Texture.hpp"
#include "util/Definitions.hpp"

#include <memory>
#include <unordered_map>

namespace tudov
{
	class Renderer;

	class TextureManager
	{
	  protected:
		TextureID _latestID;
		std::unordered_map<TextureID, std::shared_ptr<Texture>> _textures;

	  public:
		explicit TextureManager() noexcept;
		explicit TextureManager(const TextureManager &) noexcept = default;
		explicit TextureManager(TextureManager &&) noexcept = default;
		TextureManager &operator=(const TextureManager &) noexcept = default;
		TextureManager &operator=(TextureManager &&) noexcept = default;
		~TextureManager() noexcept = default;

		std::shared_ptr<Texture> GetTexture(TextureID textureID) noexcept;
		std::tuple<std::shared_ptr<Texture>, TextureID> LoadTexture(Renderer &renderer) noexcept;
	};
} // namespace tudov
