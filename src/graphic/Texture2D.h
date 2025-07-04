#pragma once

#include "bgfx/bgfx.h"
#include "glm/ext/vector_float2.hpp"
#include "util/Log.h"

#include <cstdint>

struct SDL_Surface;

namespace tudov
{
	class Texture2D
	{
	  private:
		std::shared_ptr<Log> _log;
		bgfx::TextureHandle _textureHandle;
		std::uint32_t _width;
		std::uint32_t _height;

	  public:
		explicit Texture2D(const SDL_Surface &surface);
		explicit Texture2D();
		~Texture2D() noexcept;

	  private:
		std::int32_t GetTextureMemorySizeFromSurface(const SDL_Surface &surface) noexcept;

	  public:
		bgfx::TextureHandle GetHandle() noexcept;
		std::uint32_t GetWidth() const noexcept;
		std::uint32_t GetHeight() const noexcept;
		std::tuple<std::uint32_t, std::uint32_t> GetSize() const noexcept;
		void SetSample() noexcept;
	};
} // namespace tudov
