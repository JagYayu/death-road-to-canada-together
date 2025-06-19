#include "BitmapFont.h"

#include "SDL3/SDL_rect.h"
#include <algorithm>
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STBCC_GRID_COUNT_X_LOG2 6
#define STBCC_GRID_COUNT_Y_LOG2 6
#define STB_CONNECTED_COMPONENTS_IMPLEMENTATION
#include "stb_connected_components.h"

#include <vector>

using namespace tudov;

BitmapFont::BitmapFont(std::string_view fontData)
    : _width(0),
      _height(0),
      _channels(0),
      _glyphWidth(0),
      _glyphHeight(0),
      _columns(0)
{
	int w, h, channels;
	unsigned char *img = stbi_load_from_memory(
	    reinterpret_cast<const unsigned char *>(fontData.data()),
	    fontData.size(), &w, &h, &channels, 4);

	if (!img)
		throw std::runtime_error("Failed to load font image");

	int N = w * h;
	auto getpix = [&](int i) -> uint32_t
	{
		const unsigned char *p = img + 4 * i;
		return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
	};

	uint32_t bg = getpix(0);
	std::vector<uint8_t> mask(N);
	for (int i = 0; i < N; ++i)
		mask[i] = (getpix(i) != bg);

	std::vector<int> label(N, 0);
	int nextId = 1;
	struct BBox
	{
		int minx, miny, maxx, maxy;
	};
	std::vector<BBox> bboxes(1);

	std::vector<int> stack;
	stack.reserve(512);

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			int idx = y * w + x;
			if (mask[idx] && label[idx] == 0)
			{
				label[idx] = nextId;
				bboxes.push_back({x, y, x, y});
				stack.clear();
				stack.push_back(idx);

				while (!stack.empty())
				{
					int i0 = stack.back();
					stack.pop_back();
					int x0 = i0 % w, y0 = i0 / w;
					auto &bb = bboxes[nextId];
					bb.minx = std::min(bb.minx, x0);
					bb.miny = std::min(bb.miny, y0);
					bb.maxx = std::max(bb.maxx, x0);
					bb.maxy = std::max(bb.maxy, y0);

					const int dx[4] = {1, -1, 0, 0}, dy[4] = {0, 0, 1, -1};
					for (int d = 0; d < 4; ++d)
					{
						int xn = x0 + dx[d], yn = y0 + dy[d];
						if (xn >= 0 && xn < w && yn >= 0 && yn < h)
						{
							int in = yn * w + xn;
							if (mask[in] && label[in] == 0)
							{
								label[in] = nextId;
								stack.push_back(in);
							}
						}
					}
				}
				nextId++;
			}
		}
	}

	struct Item
	{
		int id, minx, miny;
	};

	std::vector<Item> items;
	for (int id = 1; id < nextId; ++id)
		items.push_back({id, bboxes[id].minx, bboxes[id].miny});

	// 按行优先 (miny), 行内左到右 (minx)
	std::sort(items.begin(), items.end(), [](const Item &a, const Item &b)
	{
		if (a.miny != b.miny)
			return a.miny < b.miny;
		return a.minx < b.minx;
	});

	std::unordered_map<char, SDL_Rect> map;
	for (int idx = 0; idx < (int)items.size(); ++idx)
	{
		int id = items[idx].id;
		auto &bb = bboxes[id];

		// 你可以改为自己映射，比如只映射 0-9 A-Z a-z
		// char ch = (idx < 95 ? char(32 + idx) : '?'); // ASCII 32~126
		// map[ch] = {bb.minx, bb.miny, bb.maxx - bb.minx + 1, bb.maxy - bb.miny + 1};
		map[idx] = {bb.minx, bb.miny, bb.maxx - bb.minx + 1, bb.maxy - bb.miny + 1};
	}

	stbi_image_free(img);
	_charMap = std::move(map);
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

const std::unordered_map<char, SDL_Rect> &BitmapFont::GetCharMap() const noexcept
{
	return _charMap;
}

bool BitmapFont::HasChar(char c) const
{
	return _charMap.contains(c);
}

SDL_Rect BitmapFont::GetCharRect(char c) const
{
	auto &&it = _charMap.find(c);
	if (it != _charMap.end())
	{
		return it->second;
	}
	return SDL_Rect();
}
