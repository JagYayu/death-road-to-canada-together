#pragma once

#include "program/IEngineComponent.h"
#include "resource/TextureManager.hpp"
#include "util/Defs.h"
#include "util/Log.h"

#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace tudov
{
	class Window;

	struct IRenderer
	{
		virtual void SetRenderTarget(const std::shared_ptr<Texture> &texture = nullptr) noexcept = 0;
		virtual void DrawTexture(const std::shared_ptr<Texture> &texture, const SDL_FRect &dst, const SDL_FRect &src, std::float_t z) noexcept = 0;

		virtual void Clear(const SDL_Color &color) noexcept = 0;
		virtual void Render() noexcept = 0;
	};

	class Renderer : public IRenderer
	{
		friend class LuaAPI;

	  private:
		struct DrawTextureCommand
		{
			enum Nulls : std::uint8_t
			{
				Src = 1 << 0,
				Ori = 1 << 1,
			} nulls;
			SDL_Texture *tex;
			SDL_FRect dst;
			SDL_FRect src;
			SDL_FPoint ori;
			std::float_t ang;
		};

		struct DrawRectCommand
		{
			// TODO
		};

		enum class RenderCommandType : std::uint8_t
		{
			DrawTexture,
			DrawRect,
		};

		struct RenderCommand
		{
			union
			{
				DrawTextureCommand drawTexture;
				DrawTextureCommand drawRect;
			};

			RenderCommandType type;
			SDL_Color color;
			std::float_t z;

			bool operator<(const RenderCommand &other) const
			{
				return z < other.z;
			}

			bool operator>(const RenderCommand &other) const
			{
				return z > other.z;
			}
		};

	  private:
		std::shared_ptr<Log> _log;
		SDL_Renderer *_sdlRenderer;
		TextureManager _textureManager;
		std::vector<RenderCommand> _renderCommands;

	  public:
		Window &window;

	  private:
		std::shared_ptr<Texture> GetOrCreateTexture(ImageID imageID) noexcept;

		void LuaDrawTexture(const sol::table &args);
		std::shared_ptr<Texture> LuaNewRenderTexture(std::int32_t width, std::int32_t height);
		void LuaClear(std::uint32_t color) noexcept;

	  public:
		explicit Renderer(Window &window) noexcept;
		~Renderer() noexcept = default;

		void Initialize() noexcept;

		void SetRenderTarget(const std::shared_ptr<Texture> &texture = nullptr) noexcept override;
		void DrawTexture(const std::shared_ptr<Texture> &texture, const SDL_FRect &dst, const SDL_FRect &src, std::float_t z) noexcept override;

		void Clear(const SDL_Color &color) noexcept override;
		void Render() noexcept override;

		SDL_Renderer *GetSDLRendererHandle() noexcept;
		const SDL_Renderer *GetSDLRendererHandle() const noexcept;
	};
} // namespace tudov
