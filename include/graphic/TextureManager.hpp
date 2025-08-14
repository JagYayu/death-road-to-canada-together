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

		std::shared_ptr<Texture> GetTexture(TextureID textureID) noexcept;
		std::tuple<std::shared_ptr<Texture>, TextureID> LoadTexture(Renderer &renderer) noexcept;
	};
} // namespace tudov
