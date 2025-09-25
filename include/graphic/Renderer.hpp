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
#include "program/Context.hpp"
#include "system/Log.hpp"
#include "util/Color.hpp"
#include "util/Definitions.hpp"

#include "SDL3/SDL_rect.h"
#include "sol/forward.hpp"
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
	enum class EBlendMode : std::uint8_t;

	struct IWindow;
	struct DrawRectArgs;
	struct DrawTextArgs;

	class RenderBuffer;
	class RenderTarget;
	class Window;

	struct IRenderer
	{
		virtual ~IRenderer() noexcept = default;

		virtual IWindow &GetWindow() noexcept = 0;

		virtual void InitializeRenderer() noexcept = 0;
		virtual void DeinitializeRenderer() noexcept = 0;

		virtual EVSyncMode GetVSync() noexcept = 0;
		virtual bool SetVSync(EVSyncMode mode) noexcept = 0;

		virtual void DrawRect(Texture *texture, const SDL_FRect &dst, const SDL_FRect *src = nullptr, Color color = Color::White) = 0;
		virtual void DrawDebugText(std::float_t x, std::float_t y, std::string_view text, Color color = Color::White) = 0;
		virtual SDL_FRect DrawText(DrawTextArgs *args) = 0;
		virtual SDL_FRect DrawRichText(DrawTextArgs *args) = 0;

		virtual EBlendMode GetBlendMode(Texture *texture) noexcept = 0;
		virtual void SetBlendMode(Texture *texture, EBlendMode blendMode) noexcept = 0;

		virtual void Clear() noexcept = 0;
		virtual void Reset() noexcept = 0;
		virtual void Begin() noexcept = 0;
		virtual void End() noexcept = 0;
	};

	class Renderer : public IRenderer, public IContextProvider, ILogProvider
	{
		friend class LuaBindings;
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
		explicit Renderer(const Renderer &) noexcept = default;
		explicit Renderer(Renderer &&) noexcept = default;
		Renderer &operator=(const Renderer &) noexcept = delete;
		Renderer &operator=(Renderer &&) noexcept = delete;
		~Renderer() noexcept = default;

		Log &GetLog() noexcept override;

		void InitializeRenderer() noexcept override;
		void DeinitializeRenderer() noexcept override;

		IWindow &GetWindow() noexcept override;
		Context &GetContext() noexcept override;

		EVSyncMode GetVSync() noexcept override;
		bool SetVSync(EVSyncMode mode) noexcept override;

		void DrawRect(Texture *texture, const SDL_FRect &dst, const SDL_FRect *src = nullptr, Color color = Color::White) override;
		void DrawDebugText(std::float_t x, std::float_t y, std::string_view text, Color color = Color::White) override;
		SDL_FRect DrawText(DrawTextArgs *args) override;
		SDL_FRect DrawRichText(DrawTextArgs *args) override;
		void DrawVertices(Texture *texture, const std::vector<SDL_Vertex> &vertices);
		void DrawVertices(Texture *texture, const std::vector<SDL_Vertex> &vertices, const std::vector<std::int32_t> &indices);

		EBlendMode GetBlendMode(Texture *texture) noexcept override;
		void SetBlendMode(Texture *texture, EBlendMode blendMode) noexcept override;

		void Clear() noexcept override;
		void Reset() noexcept override;
		void Begin() noexcept override;
		void End() noexcept override;

		std::shared_ptr<RenderTarget> NewRenderTarget(std::int32_t width, std::int32_t height) noexcept;
		void BeginTarget(const std::shared_ptr<RenderTarget> &renderTarget) noexcept;
		std::shared_ptr<RenderTarget> EndTarget() noexcept;

		std::shared_ptr<RenderBuffer> NewRenderBuffer() noexcept;

		std::shared_ptr<Texture> ExtractTexture(sol::table args) noexcept;

		SDL_Renderer *GetSDLRendererHandle() noexcept;

		inline const SDL_Renderer *GetSDLRendererHandle() const noexcept
		{
			return const_cast<Renderer *>(this)->GetSDLRendererHandle();
		}

	  protected:
		void SetRenderTexture(const std::shared_ptr<Texture> &texture = nullptr) noexcept;

	  private:
		std::shared_ptr<Texture> GetOrCreateImageTexture(ImageID imageID);

		void LuaBeginTarget(sol::object renderTarget) noexcept;
		std::shared_ptr<RenderTarget> LuaEndTarget() noexcept;
		void LuaDrawRect(DrawRectArgs *args) noexcept;
		void LuaDrawDebugText(std::double_t x, std::double_t y, sol::string_view text) noexcept;
		std::tuple<std::float_t, std::float_t, std::float_t, std::float_t> LuaDrawText(sol::object args) noexcept;
		std::tuple<std::float_t, std::float_t, std::float_t, std::float_t> LuaDrawRichText(sol::object args) noexcept;
		std::shared_ptr<Texture> LuaDrawExtractTexture(sol::table args) noexcept;
		std::shared_ptr<RenderTarget> LuaNewRenderTarget(sol::object width = sol::nil, sol::object height = sol::nil);
		void LuaClear() noexcept;
		std::tuple<std::float_t, std::float_t> LuaGetTargetSize(const std::shared_ptr<RenderTarget> &renderTarget) noexcept;

		void ApplyTransform(SDL_FRect &dst) noexcept;

		void SDLRenderClear() noexcept;
		void SDLRenderPresent() noexcept;
		void SDLRenderTexture(SDL_Texture *sdlTexture, const SDL_FRect *srcRect, const SDL_FRect *dstRect) noexcept;
		void SDLSetRenderDrawColor(std::float_t r, std::float_t g, std::float_t b, std::float_t a) noexcept;
		void SDLSetRenderTarget(SDL_Texture *sdlTexture) noexcept;

		void TryLogSDLError(bool value) noexcept;
	};
} // namespace tudov
