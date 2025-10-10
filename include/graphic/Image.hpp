/**
 * @file graphic/Image.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Resource/Resource.hpp"

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
		std::string_view _path;
		impl::SDL_Surface *_sdlSurface;

	  public:
		explicit Image(std::string_view path, const std::vector<std::byte> &bytes);
		explicit Image(const Image &) noexcept = default;
		explicit Image(Image &&) noexcept = default;
		Image &operator=(const Image &) noexcept = default;
		Image &operator=(Image &&) noexcept = default;
		~Image() noexcept;

		std::string_view GetFilePath() const noexcept override;

		bool IsValid() noexcept;
		void Initialize(std::string_view memory) noexcept;

		impl::SDL_Surface *GetSDLSurfaceHandle() noexcept;
		const impl::SDL_Surface *GetSDLSurfaceHandle() const noexcept;
	};
} // namespace tudov
