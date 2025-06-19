#include "SDLRenderBuffer.h"

#include "SDLRenderer.h"
#include "SDLTexture.h"
#include "SDLTrueTypeFont.h"
#include "graphic/Window.h"
#include "program/Engine.h"
#include "sol/types.hpp"
#include "util/StringUtils.hpp"
#include "util/Utils.hpp"

#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "glm/ext/matrix_float3x3.hpp"

#include <cmath>
#include <exception>
#include <memory>
#include <stdexcept>

using namespace tudov;

SDLRenderBuffer::SDLRenderBuffer(SDLRenderer &renderer) noexcept
    : IRenderBuffer(renderer),
      _log(Log::Get("SDLRenderBuffer")),
      _transform(),
      _textEngine(TTF_CreateRendererTextEngine(renderer.GetRaw()))
{
}

glm::mat3x3 &SDLRenderBuffer::GetTransform() noexcept
{
	return _transform;
}

void SDLRenderBuffer::SetTransform(const glm::mat3x3 &transform) noexcept
{
	_transform = transform;
}

sol::object SDLRenderBuffer::LuaGetTransform() noexcept
{
	try
	{
		auto &&tbl = renderer.window.engine.modManager.scriptEngine.CreateTable();
		tbl[1] = _transform[0][0];
		tbl[2] = _transform[1][0];
		tbl[3] = _transform[2][0];
		tbl[4] = _transform[0][1];
		tbl[5] = _transform[1][1];
		tbl[6] = _transform[2][1];
		tbl[7] = _transform[0][2];
		tbl[8] = _transform[1][2];
		tbl[9] = _transform[2][2];
		return tbl;
	}
	catch (std::exception &e)
	{
		UnhandledCppException(_log, e);
		return sol::nil;
	}
}

void SDLRenderBuffer::LuaSetTransform(const sol::table &mat3) noexcept
{
	try
	{
		_transform = glm::mat3{
		    mat3.get<float>(1),
		    mat3.get<float>(4),
		    mat3.get<float>(7),
		    mat3.get<float>(2),
		    mat3.get<float>(5),
		    mat3.get<float>(8),
		    mat3.get<float>(3),
		    mat3.get<float>(6),
		    mat3.get<float>(9),
		};
	}
	catch (std::exception &e)
	{
		UnhandledCppException(_log, e);
	}
}

void SDLRenderBuffer::Clear() noexcept
{
	_commandQueue.clear();
}

void SDLRenderBuffer::Draw(std::float_t x, std::float_t y, std::float_t w, std::float_t h, ResourceID t, std::float_t tx, std::float_t ty, std::float_t tw, std::float_t th,
                           Color color, std::float_t a, std::float_t cx, std::float_t cy, std::float_t z)
{
	auto &&texRes = renderer.window.engine.textureManager.GetResource(t);
	if (!texRes)
	{
		auto &&msg = std::format("Texture not found: <{}>", t);
		throw std::exception(msg.c_str());
	}

	auto &&tex = std::dynamic_pointer_cast<SDLTexture>(texRes);
	if (!tex)
	{
		auto &&msg = std::format("Invalid texture type");
		throw std::exception(msg.c_str());
	}

	glm::vec3 tl = _transform * glm::vec3(x, y, 1.0f);
	glm::vec3 tr = _transform * glm::vec3(x + w, y, 1.0f);
	glm::vec3 bl = _transform * glm::vec3(x, y + h, 1.0f);
	glm::vec3 br = _transform * glm::vec3(x + w, y + h, 1.0f);
	float minX = std::min({tl.x, tr.x, bl.x, br.x});
	float minY = std::min({tl.y, tr.y, bl.y, br.y});
	float maxX = std::max({tl.x, tr.x, bl.x, br.x});
	float maxY = std::max({tl.y, tr.y, bl.y, br.y});
	float newW = maxX - minX;
	float newH = maxY - minY;
	glm::vec3 transformedCtr = _transform * glm::vec3(cx, cy, 1.0f);
	SDL_FPoint relativeCtr = SDL_FPoint{transformedCtr.x - minX, transformedCtr.y - minY};

	_commandQueue.push_back(Command{
	    .tex = tex->GetRaw(),
	    .dst = SDL_FRect{minX, minY, newW, newH},
	    .src = SDL_FRect{tx, ty, tw, th},
	    .ang = a,
	    .ctr = relativeCtr,
	});
}

#ifdef DrawText
#undef DrawText
#endif

SDL_FRect SDLRenderBuffer::DrawText(std::string_view text, const std::vector<FontID> &fonts, std::float_t x, std::float_t y, Color color,
                                    std::float_t scale, std::float_t alignX, std::float_t alignY, std::float_t wrapWidth, std::float_t shadow)
{
	SDL_FRect textRect{x, y, 0, 0};

	if (fonts.empty())
	{
		return textRect;
	}

	auto &fontManager = renderer.window.engine.fontManager;
	auto &textureManager = renderer.window.engine.textureManager;

	// TODO For now only support first font, loop for fallback
	auto fontID = fonts[0];

	auto font = fontManager.GetResource(fontID);
	auto &&bmp = fontManager.GetBitmapFont(fontID);
	if (!bmp.has_value())
	{
		throw std::runtime_error(std::format("Invalid font resource <{}>", fontID));
	}

	std::u32string utf32str = Utf8ToTtf32(text);

	std::float_t lineX = 0;
	std::float_t lineY = 0;
	std::float_t lineH = 0;
	std::float_t maxW = 0;

	for (char32_t c : utf32str)
	{
		if (c == U'\n')
		{
			lineX = 0;
			lineY += lineH;
			lineH = 0;
			continue;
		}

		bool drawn = false;

		auto &&info = bmp.value().get();
		auto &&charMap = info.GetCharMap();
		auto columns = info.GetColumns();
		auto &&charRect = info.GetCharRect(c);

		auto &&it = charMap.find(c);
		if (it != charMap.end())
		{
			auto texRes = textureManager.GetResource(fontID);
			auto tex = std::dynamic_pointer_cast<SDLTexture>(texRes);

			std::float_t dstW = charRect.w * scale;
			std::float_t dstH = charRect.h * scale;

			std::float_t drawX = x + lineX;
			std::float_t drawY = y + lineY;

			if (shadow > 0)
			{
				Draw(drawX + shadow, drawY + shadow, dstW, dstH,
				     fontID, charRect.x, charRect.y, charRect.w, charRect.h, color,
				     0.f, 0.f, 0.f, 0.f);
			}
			else if (shadow < 0)
			{
				int s = -shadow;
				for (int dx = -s; dx <= s; ++dx)
				{
					for (int dy = -s; dy <= s; ++dy)
					{
						if (dx != 0 || dy != 0)
						{
							Draw(drawX + dx, drawY + dy, dstW, dstH,
							     fontID, charRect.x, charRect.y, charRect.w, charRect.h, color,
							     0.f, 0.f, 0.f, 0.f);
						}
					}
				}
			}

			Draw(drawX, drawY, dstW, dstH,
			     fontID, charRect.x, charRect.y, charRect.w, charRect.h, color,
			     0.f);

			lineX += dstW;
			lineH = std::max(lineH, dstH);
			maxW = std::max(maxW, lineX);
			drawn = true;
		}

		// if (!drawn && font)
		// {
		// 	auto &sdlFont = *std::dynamic_pointer_cast<SDLTrueTypeFont>(font);

		// 	SDL_Surface *surf = TTF_RenderGlyph_Solid(static_cast<TTF_Font *>(sdlFont.GetRaw()), c, SDL_Color{
		// 	                                                                                            static_cast<Uint8>((color >> 16) & 0xFF),
		// 	                                                                                            static_cast<Uint8>((color >> 8) & 0xFF),
		// 	                                                                                            static_cast<Uint8>(color & 0xFF),
		// 	                                                                                            static_cast<Uint8>((color >> 24) & 0xFF),
		// 	                                                                                        });
		// 	if (surf)
		// 	{
		// 		auto rawRenderer = (static_cast<SDLRenderer &>(renderer)).GetRaw();

		// 		SDL_Texture *glyphTex = SDL_CreateTextureFromSurface(rawRenderer, surf);

		// 		if (glyphTex)
		// 		{
		// 			float dstW = surf->w * scale;
		// 			float dstH = surf->h * scale;
		// 			float drawX = x + lineX;
		// 			float drawY = y + lineY;

		// 			SDL_FRect dstRect{drawX, drawY, dstW, dstH};

		// 			if (shadow > 0)
		// 			{
		// 				SDL_FRect shadowDst = dstRect;
		// 				shadowDst.x += shadow;
		// 				shadowDst.y += shadow;
		// 				SDL_RenderTexture(rawRenderer, glyphTex, nullptr, &shadowDst);
		// 			}
		// 			else if (shadow < 0)
		// 			{
		// 				int s = -shadow;
		// 				for (int dx = -s; dx <= s; ++dx)
		// 				{
		// 					for (int dy = -s; dy <= s; ++dy)
		// 					{
		// 						if (dx == 0 && dy == 0)
		// 							continue;
		// 						SDL_FRect outlineDst = dstRect;
		// 						outlineDst.x += dx;
		// 						outlineDst.y += dy;
		// 						SDL_RenderTexture(rawRenderer, glyphTex, nullptr, &outlineDst);
		// 					}
		// 				}
		// 			}

		// 			SDL_RenderTexture(rawRenderer, glyphTex, nullptr, &dstRect);

		// 			lineX += dstW;

		// 			lineH = std::max(lineH, dstH);
		// 			maxW = std::max(maxW, lineX);

		// 			SDL_DestroyTexture(glyphTex);
		// 		}

		// 		SDL_DestroySurface(surf);
		// 	}
		// }
	}

	textRect.x = x;
	textRect.y = y;
	textRect.w = maxW;
	textRect.h = lineY + lineH;

	return textRect;
}

void SDLRenderBuffer::Sort() noexcept
{
	std::sort(_commandQueue.begin(), _commandQueue.end(), [](const auto &lhs, const auto &rhs)
	{
		return lhs.tex < rhs.tex;
	});

	SDL_Texture *currentTexture = nullptr;
}

void SDLRenderBuffer::Render() noexcept
{
	auto &&sdlRenderer = static_cast<SDLRenderer &>(renderer);

	for (const auto &cmd : _commandQueue)
	{
		SDL_RenderTextureRotated(sdlRenderer.GetRaw(), cmd.tex, &cmd.src, &cmd.dst, cmd.ang, &cmd.ctr, SDL_FLIP_NONE);
	}
}
