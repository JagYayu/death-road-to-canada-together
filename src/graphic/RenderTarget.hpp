#pragma once

#include "SDL3/SDL_rect.h"

#include <cmath>
#include <memory>

namespace tudov
{
	class Engine;
	class LuaAPI;
	class Renderer;
	class Texture;

	class RenderTarget
	{
		friend LuaAPI;
		friend Renderer;

	  private:
		Renderer &_renderer;
		std::weak_ptr<Texture> _texture;
		SDL_Rect _rect;

	  public:
		explicit RenderTarget(Renderer &renderer, std::int32_t width, std::int32_t height) noexcept;
		~RenderTarget() noexcept;

	  private:
		void LuaBeginTarget() noexcept;
		void LuaEndTarget() noexcept;

	  public:
		SDL_Rect &GetRect() noexcept;
		void BeginTarget();
		void EndTarget();
		bool Resize(std::int32_t width, std::int32_t height);
		bool ResizeToFit();
		void Draw(const SDL_FRect &dst, std::float_t z = 0);
		void Draw(const SDL_FRect &dst, const SDL_FRect &src, std::float_t z = 0);

		inline const SDL_Rect &GetRect() const noexcept
		{
			return _rect;
		}
	};
} // namespace tudov
