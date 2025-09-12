/**
 * @file graphic/Font.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <span>
#include <string>
#include <unordered_map>

struct TTF_Font;
struct SDL_Renderer;
struct SDL_Texture;

namespace tudov
{
	class Font
	{
	  private:
		struct Key
		{
			SDL_Renderer *sdlRenderer;
			std::uint16_t characterSize;
			std::int32_t maxWidth;
			std::string text;

			inline bool operator==(const Key &other) const
			{
				return sdlRenderer == other.sdlRenderer && characterSize == other.characterSize && maxWidth == other.maxWidth && text == other.text;
			}
		};

		struct KeyHash
		{
			inline size_t operator()(const Key &key) const
			{
				return std::hash<SDL_Renderer *>()(key.sdlRenderer) ^ std::hash<std::uint16_t>()(key.characterSize) ^ std::hash<std::int32_t>()(key.maxWidth) ^ std::hash<std::string>()(key.text);
			}
		};

		struct Entry
		{
			Key key;
			SDL_Texture *texture;
			std::int32_t width;
			std::int32_t height;
		};

	  private:
		std::span<const std::byte> _bytes;
		std::unordered_map<std::uint16_t, TTF_Font *> _fonts;
		std::list<Entry> _lruList;
		std::unordered_map<Key, std::list<Entry>::iterator, KeyHash> _lruMap;

	  public:
		explicit Font(std::string_view path, const std::vector<std::byte> &bytes) noexcept;
		~Font() noexcept;

		std::tuple<SDL_Texture *, std::int32_t, std::int32_t> GetTextInfo(SDL_Renderer *sdlRenderer, std::uint16_t characterSize, std::string_view text, std::int32_t maxWidth) noexcept;
	};
} // namespace tudov
