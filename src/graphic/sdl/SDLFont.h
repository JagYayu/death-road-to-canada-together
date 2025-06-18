#pragma once

#include "graphic/IFont.h"

#include <SDL3_ttf/SDL_ttf.h>
#include <string_view>

namespace tudov
{
	class SDLFont : public IFont
	{
	  private:

		
	  public:
		explicit SDLFont(std::string_view data, std::size_t size);
		~SDLFont() noexcept;

		TTF_Font *GetRaw() const
		{
			return font;
		}

	  private:
		TTF_Font *font = nullptr;
	};
} // namespace tudov
