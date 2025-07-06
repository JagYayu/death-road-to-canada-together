#pragma once

#include "resource/TextureManager.hpp"
#include "util/Log.h"

#include "SDL3/SDL_render.h"

#include <memory>

namespace tudov
{
	class Window;

	class Renderer
	{
		friend class LuaAPI;

	  public:
		struct RenderTextureCommand
		{
			
		};

	  private:
		std::shared_ptr<Log> _log;
		SDL_Renderer *_sdlRenderer;

		TextureManager textureManager;

	  public:
		Window &window;

	  private:
		std::shared_ptr<Texture> GetOrCreateTexture(ImageID imageID) noexcept;

		void LuaDrawRect(const sol::table &args);
		std::shared_ptr<Texture> LuaNewRenderTexture(std::int32_t width, std::int32_t height);

	  public:
		explicit Renderer(Window &window) noexcept;

		void Initialize() noexcept;

		void SetRenderTarget(const std::shared_ptr<Texture> &texture = nullptr) noexcept;
		void RenderTexture(const SDL_FRect &dst, const SDL_FRect &src, const std::shared_ptr<Texture> &texture = nullptr) noexcept;

		void RenderPresent() noexcept;

		SDL_Renderer *GetSDLRendererHandle() noexcept;
		const SDL_Renderer *GetSDLRendererHandle() const noexcept;
	};
} // namespace tudov
