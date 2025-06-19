#include "SDLTrueTypeFont.h"

#include <format>
#include <stdexcept>

using namespace tudov;

SDLTrueTypeFont::SDLTrueTypeFont(std::string_view data, std::size_t size)
{
	// auto rwops = SDL_IOFromConstMem(data, size);
	// if (!rwops)
	// {
	// 	throw std::runtime_error("SDL_RWFromConstMem failed");
	// }

	// font = TTF_OpenFontRW(rwops, 1, ptsize);
	// if (!font)
	// {
	// 	throw std::runtime_error(std::format("TTF_OpenFontRW failed: {}", TTF_GetError()));
	// }
}

SDLTrueTypeFont::~SDLTrueTypeFont() noexcept
{
	if (font)
	{
		TTF_CloseFont(font);
	}
}
