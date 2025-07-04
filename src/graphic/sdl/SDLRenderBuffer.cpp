// #include "SDLRenderBuffer.h"

// #include "SDLRenderer.h"
// #include "SDLTexture.h"
// #include "SDLTrueTypeFont.h"
// #include "graphic/Window.h"
// #include "program/Engine.h"
// #include "util/StringUtils.hpp"
// #include "util/Utils.hpp"

// #include "SDL3/SDL_rect.h"
// #include "SDL3/SDL_render.h"
// #include "SDL3/SDL_surface.h"
// #include "SDL3_ttf/SDL_ttf.h"
// #include "glm/ext/matrix_float3x3.hpp"
// #include "sol/types.hpp"

// #include <cmath>
// #include <exception>
// #include <memory>
// #include <stdexcept>

// using namespace tudov;

// SDLRenderBuffer::SDLRenderBuffer(SDLRenderer &renderer) noexcept
//     : IRenderBuffer(renderer),
//       _log(Log::Get("SDLRenderBuffer")),
//       _transform(),
//       _textEngine(TTF_CreateRendererTextEngine(renderer.GetRaw()))
// {
// }

// glm::mat3x3 &SDLRenderBuffer::GetTransform() noexcept
// {
// 	return _transform;
// }

// void SDLRenderBuffer::SetTransform(const glm::mat3x3 &transform) noexcept
// {
// 	_transform = transform;
// }

// sol::object SDLRenderBuffer::LuaGetTransform() noexcept
// {
// 	try
// 	{
// 		auto &&tbl = renderer.window.engine.modManager.scriptEngine.CreateTable();
// 		tbl[1] = _transform[0][0];
// 		tbl[2] = _transform[1][0];
// 		tbl[3] = _transform[2][0];
// 		tbl[4] = _transform[0][1];
// 		tbl[5] = _transform[1][1];
// 		tbl[6] = _transform[2][1];
// 		tbl[7] = _transform[0][2];
// 		tbl[8] = _transform[1][2];
// 		tbl[9] = _transform[2][2];
// 		return tbl;
// 	}
// 	catch (std::exception &e)
// 	{
// 		UnhandledCppException(_log, e);
// 		return sol::nil;
// 	}
// }

// void SDLRenderBuffer::LuaSetTransform(const sol::table &mat3) noexcept
// {
// 	try
// 	{
// 		_transform = glm::mat3{
// 		    mat3.get<float>(1),
// 		    mat3.get<float>(4),
// 		    mat3.get<float>(7),
// 		    mat3.get<float>(2),
// 		    mat3.get<float>(5),
// 		    mat3.get<float>(8),
// 		    mat3.get<float>(3),
// 		    mat3.get<float>(6),
// 		    mat3.get<float>(9),
// 		};
// 	}
// 	catch (std::exception &e)
// 	{
// 		UnhandledCppException(_log, e);
// 	}
// }

// void SDLRenderBuffer::Clear() noexcept
// {
// 	_commandQueue.clear();
// }

// void SDLRenderBuffer::Draw(std::float_t x, std::float_t y, std::float_t w, std::float_t h, ResourceID t, std::float_t tx, std::float_t ty, std::float_t tw, std::float_t th,
//                            Color color, std::float_t a, std::float_t cx, std::float_t cy, std::float_t z)
// {
// 	auto &&texRes = renderer.window.engine.textureManager.GetResource(t);
// 	if (!texRes)
// 	{
// 		auto &&msg = std::format("Texture not found: <{}>", t);
// 		throw std::exception(msg.c_str());
// 	}

// 	auto &&tex = std::dynamic_pointer_cast<SDLTexture>(texRes);
// 	if (!tex)
// 	{
// 		auto &&msg = std::format("Invalid texture type");
// 		throw std::exception(msg.c_str());
// 	}

// 	glm::vec3 tl = _transform * glm::vec3(x, y, 1.0f);
// 	glm::vec3 tr = _transform * glm::vec3(x + w, y, 1.0f);
// 	glm::vec3 bl = _transform * glm::vec3(x, y + h, 1.0f);
// 	glm::vec3 br = _transform * glm::vec3(x + w, y + h, 1.0f);
// 	float minX = std::min({tl.x, tr.x, bl.x, br.x});
// 	float minY = std::min({tl.y, tr.y, bl.y, br.y});
// 	float maxX = std::max({tl.x, tr.x, bl.x, br.x});
// 	float maxY = std::max({tl.y, tr.y, bl.y, br.y});
// 	float newW = maxX - minX;
// 	float newH = maxY - minY;
// 	glm::vec3 transformedCtr = _transform * glm::vec3(cx, cy, 1.0f);
// 	SDL_FPoint relativeCtr = SDL_FPoint{transformedCtr.x - minX, transformedCtr.y - minY};

// 	_commandQueue.push_back(Command{
// 	    .tex = tex->GetRaw(),
// 	    .dst = SDL_FRect{minX, minY, newW, newH},
// 	    .src = SDL_FRect{tx, ty, tw, th},
// 	    .ang = a,
// 	    .ctr = relativeCtr,
// 	});
// }

// struct TextMeasureBox
// {
// 	std::float_t lineX = 0;
// 	std::float_t lineY = 0;
// 	std::float_t lineH = 0;
// 	std::float_t maxW = 0;
// };

// bool DrawCharacter(SDLRenderBuffer &buffer, const IRenderBuffer::DrawTextArgs &args, TextMeasureBox &mBox, char32_t ch, std::shared_ptr<IFont> font) noexcept
// {
// 	if (!font || !font->HasChar(ch))
// 	{
// 		return false;
// 	}
// 	auto &&c = font->GetChar(ch, args.fontSize);

// 	auto texRes = buffer.renderer.window.engine.textureManager.GetResource(c.tex);
// 	auto tex = std::dynamic_pointer_cast<SDLTexture>(texRes);

// 	std::float_t dstW = c.rect.w * args.scale;
// 	std::float_t dstH = c.rect.h * args.scale;

// 	std::float_t drawX = args.x + mBox.lineX;
// 	std::float_t drawY = args.y + mBox.lineY;

// 	if (args.shadow > 0)
// 	{
// 		buffer.Draw(drawX + args.shadow, drawY + args.shadow, dstW, dstH,
// 		            c.tex, c.rect.x, c.rect.y, c.rect.w, c.rect.h, args.shadowColor,
// 		            0.f, 0.f, 0.f, args.z);
// 	}
// 	else if (args.shadow < 0)
// 	{
// 		int s = -args.shadow;
// 		for (int dx = -s; dx <= s; ++dx)
// 		{
// 			for (int dy = -s; dy <= s; ++dy)
// 			{
// 				if (dx != 0 || dy != 0)
// 				{
// 					buffer.Draw(drawX + dx, drawY + dy, dstW, dstH,
// 					            c.tex, c.rect.x, c.rect.y, c.rect.w, c.rect.h, args.shadowColor,
// 					            0.f, 0.f, 0.f, args.z);
// 				}
// 			}
// 		}
// 	}

// 	buffer.Draw(drawX, drawY, dstW, dstH,
// 	            c.tex, c.rect.x, c.rect.y, c.rect.w, c.rect.h, args.color,
// 	            0.f, 0.f, 0.f, args.z);

// 	mBox.lineX += dstW;
// 	mBox.lineH = std::max(mBox.lineH, dstH);
// 	mBox.maxW = std::max(mBox.maxW, mBox.lineX);

// 	return true;
// }

// // bool TryDrawCharacter(SDLRenderBuffer &buffer, const IRenderBuffer::DrawTextArgs &args, TextMeasureBox &mBox, char32_t ch, std::shared_ptr<IFont> font) noexcept
// // {
// // 	return font && font->HasChar(ch) && DrawCharacter(buffer, args, mBox, font->GetChar(ch, args.fontSize));
// // }

// #undef DrawText

// SDL_FRect SDLRenderBuffer::DrawText(const IRenderBuffer::DrawTextArgs &args)
// {
// 	SDL_FRect textRect{args.x, args.y, 0, 0};

// 	if (args.fonts.empty())
// 	{
// 		return textRect;
// 	}

// 	TextMeasureBox mBox{};

// 	// auto &&bitmapFontManager = renderer.window.engine.bitmapFontManager;
// 	// auto &&fontManager = renderer.window.engine.fontManager;

// 	auto &&font = renderer.window.engine.fontManager.GetResource(args.fonts[0]);
// 	TTF_Font *ttfFont = std::dynamic_pointer_cast<SDLTrueTypeFont>(font)->GetRaw(args.fontSize);
// 	TTF_TextEngine *textEngine = dynamic_cast<SDLRenderer&>(renderer).GetRawTextEngine();
// 	auto text = TTF_CreateText(textEngine, ttfFont, args.text.data(), args.text.size());

// 	// TTF_RenderText

// 	for (char32_t ch : Utf8ToTtf32(args.text))
// 	{
// 		if (ch == U'\n')
// 		{
// 			mBox.lineX = 0;
// 			mBox.lineY += mBox.lineH;
// 			mBox.lineH = 0;
// 			continue;
// 		}

		

// 		// for (auto font : args.fonts)
// 		// {
// 		// 	if (DrawCharacter(*this, args, mBox, ch, bitmapFontManager.GetResource(font)) ||
// 		// 	    DrawCharacter(*this, args, mBox, ch, fontManager.GetResource(font)))
// 		// 	{
// 		// 		break;
// 		// 	}

// 		// 	textRect.x = args.x;
// 		// 	textRect.y = args.y;
// 		// 	textRect.w = mBox.maxW;
// 		// 	textRect.h = mBox.lineY + mBox.lineH;
// 		// }
// 	}

// 	return textRect;
// }

// void SDLRenderBuffer::Sort() noexcept
// {
// 	std::sort(_commandQueue.begin(), _commandQueue.end(), [](const auto &lhs, const auto &rhs)
// 	{
// 		return lhs.tex < rhs.tex;
// 	});

// 	SDL_Texture *currentTexture = nullptr;
// }

// void SDLRenderBuffer::Render() noexcept
// {
// 	auto &&sdlRenderer = static_cast<SDLRenderer &>(renderer);

// 	for (const auto &cmd : _commandQueue)
// 	{
// 		SDL_RenderTextureRotated(sdlRenderer.GetRaw(), cmd.tex, &cmd.src, &cmd.dst, cmd.ang, &cmd.ctr, SDL_FLIP_NONE);
// 	}
// }
