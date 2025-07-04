#include "IRenderer.h"

#include "SDL3/SDL_pixels.h"

using namespace tudov;

IRenderer::IRenderer(Window &window) noexcept
    : window(window)
{
}

static IRenderer::DrawArgs drawArgs{};

void IRenderer::LuaDraw(const sol::table &args) noexcept
{
	drawArgs.tex = args.get_or("tex", 0);

	drawArgs.angle = args.get_or("angle", 0.f);
	if (auto &&center = args.get<sol::table>("center"); center.valid())
	{
		drawArgs.center.x = center.get_or(1, 0.f);
		drawArgs.center.y = center.get_or(2, 0.f);
	}
	{
		std::uint32_t color = args.get_or("color", -1);
		drawArgs.color = SDL_Color{
		    static_cast<Uint8>((color >> 24) & 0xFF),
		    static_cast<Uint8>((color >> 16) & 0xFF),
		    static_cast<Uint8>((color >> 8) & 0xFF),
		    static_cast<Uint8>(color & 0xFF)};
	}
	if (auto &&dst = args.get<sol::table>("dst"); dst.valid())
	{
		drawArgs.dst.x = dst.get_or(1, 0.f);
		drawArgs.dst.y = dst.get_or(2, 0.f);
		drawArgs.dst.w = dst.get_or(3, 0.f);
		drawArgs.dst.h = dst.get_or(4, 0.f);
	}
	if (auto &&src = args.get<sol::table>("src"); src.valid())
	{
		drawArgs.src.x = src.get_or(1, 0.f);
		drawArgs.src.y = src.get_or(2, 0.f);
		drawArgs.src.w = src.get_or(3, 0.f);
		drawArgs.src.h = src.get_or(4, 0.f);
	}
	drawArgs.z = args.get_or("z", 0.f);
}

SDL_FRect IRenderer::LuaDrawText(const sol::table &args) noexcept
{
	assert(false && "TODO");
	return {};
}
