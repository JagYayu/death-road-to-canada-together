// #include "IRenderBuffer.h"

// #include "SDL3/SDL_rect.h"
// #include "graphic/IRenderer.h"
// #include "graphic/Window.h"
// #include "program/Engine.h"
// #include "util/Defs.h"

// #include <exception>
// #include <vector>

// #ifdef DrawText
// #undef DrawText
// #endif

// using namespace tudov;

// IRenderBuffer::IRenderBuffer(IRenderer &renderer) noexcept
//     : renderer(renderer)
// {
// }

// void IRenderBuffer::LuaDraw(std::float_t x, std::float_t y, std::float_t w, std::float_t h, ResourceID t, std::float_t tx, std::float_t ty, std::float_t tw, std::float_t th,
//                             std::uint32_t color, std::float_t a, std::float_t cx, std::float_t cy, std::float_t z) noexcept
// {
// 	try
// 	{
// 		Draw(x, y, w, h, t, tx, ty, tw, th, Color(color), a, cx, cy, z);
// 	}
// 	catch (std::exception &e)
// 	{
// 		renderer.window.engine.modManager.scriptEngine.ThrowError(e.what());
// 	}
// }

// static IRenderBuffer::DrawTextArgs drawTextArgs;

// SDL_FRect IRenderBuffer::LuaDrawText(sol::string_view text, const sol::table &fonts, std::int32_t fontSize, std::float_t x, std::float_t y, std::uint32_t color,
//                                      std::float_t scale, std::float_t alignX, std::float_t alignY, std::float_t wrapWidth, std::float_t shadow) noexcept
// {
// 	try
// 	{
// 		drawTextArgs.text = text;
// 		for (auto i = 0; i < fonts.size(); ++i)
// 		{
// 			drawTextArgs.fonts.emplace_back(fonts.get<FontID>(i + 1));
// 		}
// 		drawTextArgs.fontSize = fontSize;
// 		drawTextArgs.x = x;
// 		drawTextArgs.y = y;
// 		drawTextArgs.color = Color(color);
// 		drawTextArgs.scale = scale;
// 		drawTextArgs.align = {alignX, alignY};
// 		drawTextArgs.wrapWidth = wrapWidth;
// 		drawTextArgs.shadow = shadow;
// 		SDL_FRect &&result = DrawText(drawTextArgs);
// 		drawTextArgs.fonts.clear();
// 		return result;
// 	}
// 	catch (std::exception &e)
// 	{
// 		renderer.window.engine.modManager.scriptEngine.ThrowError(e.what());
// 		return SDL_FRect();
// 	}
// }
