#pragma once

#include <string_view>

struct SDL_Surface;

namespace tudov
{
	class Image
	{
	  private:
		SDL_Surface *_sdlSurface;

	  public:
		explicit Image() noexcept;
		~Image() noexcept;

		bool IsValid() noexcept;
		void Initialize(std::string_view memory) noexcept;

		SDL_Surface *GetSDLSurfaceHandle() noexcept;
		const SDL_Surface *GetSDLSurfaceHandle()const noexcept;
	};
} // namespace tudov
