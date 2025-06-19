#pragma once

#include "graphic/IFont.h"

#include <SDL3_ttf/SDL_ttf.h>
#include <string_view>

namespace tudov
{
	class SDLTrueTypeFont : public IFont
	{
	  private:
		TTF_Font *font = nullptr;

	  public:
		explicit SDLTrueTypeFont(std::string_view data, std::size_t size);
		~SDLTrueTypeFont() noexcept;

		TTF_Font *GetRaw() const
		{
			return font;
		}
	};
} // namespace tudov
