#pragma once

#include "RenderCommand.hpp"
#include "SDL3/SDL_rect.h"

#include <cmath>
#include <memory>
#include <optional>
#include <vector>

namespace tudov
{
	class Engine;
	class LuaAPI;
	class Renderer;
	class Texture;

	class RenderTarget : public std::enable_shared_from_this<RenderTarget>
	{
		friend LuaAPI;
		friend Renderer;

	  private:
		Renderer &_renderer;
		std::shared_ptr<Texture> _texture;
		std::optional<SDL_FRect> _source;
		std::optional<SDL_FRect> _destination;
		std::vector<RenderCommand> _renderCommands;

	  public:
		explicit RenderTarget(Renderer &renderer, std::int32_t width, std::int32_t height) noexcept;
		~RenderTarget() noexcept;

	  public:
		SDL_FRect GetSource() noexcept;
		void SetSource(const std::optional<SDL_FRect> &value) noexcept;
		SDL_FRect GetDestination() noexcept;
		void SetDestination(const std::optional<SDL_FRect> &value) noexcept;
		bool Resize(std::int32_t width, std::int32_t height);
		bool ResizeToFit();

		void Draw(const SDL_FRect &dst, std::float_t z = 0);
		void Draw(const SDL_FRect &dst, const SDL_FRect &src, std::float_t z = 0);
	};
} // namespace tudov
