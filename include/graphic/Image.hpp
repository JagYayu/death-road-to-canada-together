#pragma once

#include "resource/Resource.hpp"
#include <cstddef>
#include <string_view>
#include <vector>

namespace tudov::impl
{
	using SDL_Surface = void;
}

namespace tudov
{
	class Image : public IResource
	{
	  private:
		impl::SDL_Surface *_sdlSurface;

	  public:
		explicit Image(const std::vector<std::byte> &bytes);
		~Image() noexcept;

		bool IsValid() noexcept;
		void Initialize(std::string_view memory) noexcept;

		impl::SDL_Surface *GetSDLSurfaceHandle() noexcept;
		const impl::SDL_Surface *GetSDLSurfaceHandle() const noexcept;
	};
} // namespace tudov
