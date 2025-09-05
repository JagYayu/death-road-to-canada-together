/**
 * @file graphic/Renderer.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "TextureManager.hpp"
#include "VSyncMode.hpp"
#include "mod/LuaAPI.hpp"
#include "program/Context.hpp"
#include "sol/forward.hpp"
#include "system/Log.hpp"
#include "util/Definitions.hpp"

#include "SDL3/SDL_rect.h"
#include "sol/table.hpp"

#include <cstdint>
#include <memory>
#include <stack>

struct SDL_GPUGraphicsPipeline;
struct SDL_GPUShader;
struct SDL_GPUTexture;
struct TTF_TextEngine;

namespace tudov
{
	struct IWindow;

	struct IRenderer
	{
		virtual ~IRenderer() noexcept = default;

		virtual IWindow &GetWindow() noexcept = 0;

		virtual void InitializeRenderer() noexcept = 0;
		virtual void DeinitializeRenderer() noexcept = 0;

		virtual bool ReleaseTexture(const std::shared_ptr<Texture> &texture) noexcept = 0;
		virtual std::shared_ptr<Texture> GetRenderTexture() noexcept = 0;
		virtual void SetRenderTexture(const std::shared_ptr<Texture> &texture = nullptr) noexcept = 0;

		virtual EVSyncMode GetVSync() noexcept = 0;
		virtual bool SetVSync(EVSyncMode mode) noexcept = 0;

		virtual void Draw(const std::shared_ptr<Texture> &texture, const SDL_FRect &dst, const SDL_FRect &src) = 0;
		virtual void DrawDebugText(std::float_t x, std::float_t y, std::string_view text, SDL_Color color = SDL_Color(255, 255, 255, 255)) = 0;

		virtual void Clear(const SDL_Color &color = SDL_Color(0ui8, 0ui8, 0ui8, 0ui8)) noexcept = 0;
		virtual void Reset() noexcept = 0;
		virtual void Render() noexcept = 0;
		virtual void Begin() noexcept = 0;
		virtual void End() noexcept = 0;

		void Draw(const std::shared_ptr<Texture> &texture, const SDL_FRect &dst)
		{
			auto [w, h] = texture->GetSize();
			SDL_FRect src{
			    .x = 0,
			    .y = 0,
			    .w = w,
			    .h = h,
			};
			Draw(texture, dst, src);
		}
	};

	class Window;
	class RenderTarget;

	class Renderer : public IRenderer, public IContextProvider, ILogProvider
	{
		friend class LuaAPI;
		friend RenderTarget;

	  private:
		Window &_window;
		std::shared_ptr<Log> _log;
		SDL_Renderer *_sdlRenderer;
		TTF_TextEngine *_sdlTTFTextEngine;
		std::stack<std::shared_ptr<RenderTarget>> _renderTargets;
		// Manage image textures.
		TextureManager _textureManager;
		std::unordered_map<ImageID, TextureID> _imageTextureMap;
		std::unordered_map<SDL_Texture *, std::shared_ptr<Texture>> _heldTextures;
		SDL_Texture *_sdlTextureMain;
		SDL_Texture *_sdlTextureBackground;
		bool _background;

	  public:
		explicit Renderer(Window &window) noexcept;
		~Renderer() noexcept = default;

		Log &GetLog() noexcept override;

		void InitializeRenderer() noexcept override;
		void DeinitializeRenderer() noexcept override;

		IWindow &GetWindow() noexcept override;
		Context &GetContext() noexcept override;

		std::shared_ptr<RenderTarget> NewRenderTarget(std::int32_t width, std::int32_t height) noexcept;
		void BeginTarget(const std::shared_ptr<RenderTarget> &renderTarget) noexcept;
		std::shared_ptr<RenderTarget> EndTarget() noexcept;

		bool ReleaseTexture(const std::shared_ptr<Texture> &texture) noexcept override;
		std::shared_ptr<Texture> GetRenderTexture() noexcept override;
		void SetRenderTexture(const std::shared_ptr<Texture> &texture = nullptr) noexcept override;

		EVSyncMode GetVSync() noexcept override;
		bool SetVSync(EVSyncMode mode) noexcept override;

		void Draw(const std::shared_ptr<Texture> &texture, const SDL_FRect &dst, const SDL_FRect &src) override;
		void DrawDebugText(std::float_t x, std::float_t y, std::string_view text, SDL_Color color = SDL_Color(255, 255, 255, 255)) override;

		void Clear(const SDL_Color &color = SDL_Color(0ui8, 0ui8, 0ui8, 0ui8)) noexcept override;
		void Reset() noexcept override;
		void Render() noexcept override;
		void Begin() noexcept override;
		void End() noexcept override;

		SDL_Renderer *GetSDLRendererHandle() noexcept;

		inline const SDL_Renderer *GetSDLRendererHandle() const noexcept
		{
			return const_cast<Renderer *>(this)->GetSDLRendererHandle();
		}

	  private:
		std::shared_ptr<Texture> GetOrCreateImageTexture(ImageID imageID);

		void LuaBeginTarget(sol::object renderTarget) noexcept;
		std::shared_ptr<RenderTarget> LuaEndTarget() noexcept;
		void LuaDraw(sol::table args);
		void LuaDrawDebugText(std::double_t x, std::double_t y, sol::string_view text) noexcept;
		std::shared_ptr<Texture> LuaDrawExtractTexture(sol::table args) noexcept;
		std::shared_ptr<RenderTarget> LuaNewRenderTarget(sol::object width = sol::nil, sol::object height = sol::nil);
		void LuaClear(std::uint32_t color) noexcept;
		std::tuple<std::float_t, std::float_t> LuaGetTargetSize(const std::shared_ptr<RenderTarget> &renderTarget) noexcept;

		SDL_FRect ApplyTransform(const SDL_FRect &dst) noexcept;

		void SDLRenderClear() noexcept;
		void SDLRenderPresent() noexcept;
		void SDLRenderTexture(SDL_Texture *sdlTexture, const SDL_FRect *srcRect, const SDL_FRect *dstRect) noexcept;
		void SDLSetRenderDrawColor(std::float_t r, std::float_t g, std::float_t b, std::float_t a) noexcept;
		void SDLSetRenderTarget(SDL_Texture *sdlTexture) noexcept;

		void TryLogSDLError(bool value) noexcept;
	};
} // namespace tudov
