#include "BitmapFont.h"

#include "SDL3/SDL_rect.h"
#include <algorithm>
#include <cstdint>
#include <format>
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STBCC_GRID_COUNT_X_LOG2 6
#define STBCC_GRID_COUNT_Y_LOG2 6
#define STB_CONNECTED_COMPONENTS_IMPLEMENTATION
#include "stb_connected_components.h"

#include <vector>

using namespace tudov;

BitmapFont::BitmapFont(TextureID textureID, std::string_view fontData)
    : _textureID(textureID),
      _width(0),
      _height(0),
      _channels(0),
      _glyphWidth(0),
      _glyphHeight(0),
      _columns(0)
{
	std::int32_t w;
	std::int32_t h;
	std::int32_t channels;
	unsigned char *img = stbi_load_from_memory(
	    reinterpret_cast<const unsigned char *>(fontData.data()),
	    fontData.size(), &w, &h, &channels, 4);

	if (!img)
	{
		throw std::runtime_error("Failed to load bitmap font");
	}

	int size = w * h;
	auto &&getPixel = [&](int i) constexpr -> uint32_t
	{
		const unsigned char *p = img + 4 * i;
		return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
	};

	std::vector<uint8_t> mask(size);
	{
		uint32_t bg = getPixel(0);
		for (int i = 0; i < size; ++i)
		{
			mask[i] = (getPixel(i) != bg);
		}
	}

	std::int32_t nextID = 1;
	std::vector<std::int32_t> label(size, 0);
	std::vector<BoundingBox> boxes{1};
	std::vector<int> stack;
	stack.reserve(512);

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			int idx = y * w + x;
			if (mask[idx] && label[idx] == 0)
			{
				label[idx] = nextID;
				boxes.push_back({x, y, x, y});
				stack.clear();
				stack.push_back(idx);

				while (!stack.empty())
				{
					auto i0 = stack.back();
					stack.pop_back();
					auto x0 = i0 % w;
					auto y0 = i0 / w;
					auto &b = boxes[nextID];
					b.minX = std::min(b.minX, x0);
					b.minY = std::min(b.minY, y0);
					b.maxX = std::max(b.maxX, x0);
					b.maxY = std::max(b.maxY, y0);

					const std::int32_t dx[4] = {1, -1, 0, 0};
					const std::int32_t dy[4] = {0, 0, 1, -1};
					for (int d = 0; d < 4; ++d)
					{
						int xn = x0 + dx[d], yn = y0 + dy[d];
						if (xn >= 0 && xn < w && yn >= 0 && yn < h)
						{
							int in = yn * w + xn;
							if (mask[in] && label[in] == 0)
							{
								label[in] = nextID;
								stack.push_back(in);
							}
						}
					}
				}
				++nextID;
			}
		}
	}

	std::vector<Item> items;
	for (auto id = 1; id < nextID; ++id)
	{
		items.push_back({id, boxes[id].minX, boxes[id].minY});
	}

	std::sort(items.begin(), items.end(), [](const Item &a, const Item &b) constexpr
	{
		if (a.minY != b.minY)
		{
			return a.minY < b.minY;
		}
		return a.minX < b.minX;
	});

	std::unordered_map<char32_t, SDL_Rect> map;
	for (auto idx = 0; idx < (int)items.size(); ++idx)
	{
		auto id = items[idx].id;
		auto &b = boxes[id];
		map[idx] = {b.minX, b.minY, b.maxX - b.minX + 1, b.maxY - b.minY + 1};
	}

	stbi_image_free(img);
	_charMap = std::move(map);
}

TextureID BitmapFont::GetTextureID() const noexcept
{
	return _textureID;
}

std::int32_t BitmapFont::GetWidth() const noexcept
{
	return _width;
}

std::int32_t BitmapFont::GetHeight() const noexcept
{
	return _height;
}

std::int32_t BitmapFont::GetChannels() const noexcept
{
	return _channels;
}

std::int32_t BitmapFont::GetGlyphWidth() const noexcept
{
	return _glyphWidth;
}

std::int32_t BitmapFont::GetGlyphHeight() const noexcept
{
	return _glyphHeight;
}

std::int32_t BitmapFont::GetColumns() const noexcept
{
	return _columns;
}

// const std::unordered_map<char32_t, SDL_Rect> &BitmapFont::GetCharMap() const noexcept
// {
// 	return _charMap;
// }

bool BitmapFont::HasChar(char32_t c) const
{
	return _charMap.contains(c);
}

BitmapFont::Char BitmapFont::GetChar(char32_t ch, size_t sz)
{
	auto &&it = _charMap.find(ch);
	if (it != _charMap.end())
	{
		return {
		    .tex = _textureID,
		    .rect = it->second,
		};
	}
	throw std::runtime_error(std::format("Character '{}' does not exist in bitmap font", (uint32_t)ch));
}
