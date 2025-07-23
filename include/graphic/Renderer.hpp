#pragma once

#include "RenderCommand.hpp"
#include "SDL3/SDL_rect.h"
#include "VSyncMode.hpp"
#include "mod/LuaAPI.hpp"
#include "program/Context.hpp"
#include "resource/TextureManager.hpp"
#include "util/Definitions.hpp"
#include "util/Log.hpp"

#include "sol/table.hpp"

#include <cstdint>
#include <memory>
#include <stack>
#include <vector>

struct SDL_GPUGraphicsPipeline;
struct SDL_GPUShader;
struct SDL_GPUTexture;

namespace tudov
{
	struct IWindow;

	struct IRenderer
	{
		virtual ~IRenderer() noexcept = default;

		virtual IWindow &GetWindow() noexcept = 0;

		virtual bool ReleaseTexture(const std::shared_ptr<Texture> &texture) noexcept = 0;
		virtual std::shared_ptr<Texture> GetRenderTexture() noexcept = 0;
		virtual void SetRenderTexture(const std::shared_ptr<Texture> &texture = nullptr) noexcept = 0;

		virtual EVSyncMode GetVSync() noexcept = 0;
		virtual bool SetVSync(EVSyncMode value) noexcept = 0;

		virtual void DrawRect(const std::shared_ptr<Texture> &texture, const SDL_FRect &dst, const SDL_FRect &src, std::float_t z) noexcept = 0;

		virtual void Clear(const SDL_Color &color = SDL_Color(0ui8, 0ui8, 0ui8, 0ui8)) noexcept = 0;
		virtual void Reset() noexcept = 0;
		virtual void Render() noexcept = 0;
		virtual void Begin() noexcept = 0;
		virtual void End() noexcept = 0;
	};

	class Window;
	class RenderTarget;

	class Renderer : public IRenderer, public IContextProvider
	{
		friend class LuaAPI;
		friend RenderTarget;

	  private:
		Window &_window;
		std::shared_ptr<Log> _log;
		SDL_Renderer *_sdlRenderer;
		std::stack<std::shared_ptr<RenderTarget>> _renderTargets;
		// Manage image textures.
		TextureManager _textureManager;
		std::unordered_map<ImageID, TextureID> _imageTextureMap;
		std::unordered_map<SDL_Texture *, std::shared_ptr<Texture>> _heldTextures;
		std::vector<RenderCommand> _renderCommands;
		SDL_Texture *_sdlTextureMain;
		SDL_Texture *_sdlTextureBackground;

	  public:
		explicit Renderer(Window &window) noexcept;
		~Renderer() noexcept = default;

		void Initialize() noexcept;

		IWindow &GetWindow() noexcept override;
		Context &GetContext() noexcept override;

		std::shared_ptr<RenderTarget> NewRenderTarget(std::int32_t width, std::int32_t height) noexcept;
		void BeginTarget(const std::shared_ptr<RenderTarget> &renderTarget) noexcept;
		bool EndTarget(const std::optional<SDL_FRect> &source = std::nullopt, const std::optional<SDL_FRect> &destination = std::nullopt) noexcept;

		bool ReleaseTexture(const std::shared_ptr<Texture> &texture) noexcept override;
		std::shared_ptr<Texture> GetRenderTexture() noexcept override;
		void SetRenderTexture(const std::shared_ptr<Texture> &texture = nullptr) noexcept override;

		EVSyncMode GetVSync() noexcept override;
		bool SetVSync(EVSyncMode mode) noexcept override;

		void DrawRect(const std::shared_ptr<Texture> &texture, const SDL_FRect &dst, const SDL_FRect &src, std::float_t z) noexcept override;

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
		void RenderImpl(std::vector<RenderCommand> &renderCommands) noexcept;

		void LuaBeginTarget(const std::shared_ptr<RenderTarget> &renderTarget) noexcept;
		bool LuaEndTarget(const sol::object &source, const sol::object &destination) noexcept;
		void LuaDraw(const sol::table &args);
		std::shared_ptr<RenderTarget> LuaNewRenderTarget(const sol::object &width = sol::nil, const sol::object &height = sol::nil);
		void LuaClear(std::uint32_t color) noexcept;
		std::tuple<std::float_t, std::float_t> LuaGetTargetSize(const std::shared_ptr<RenderTarget> &renderTarget) noexcept;

		void ApplyTransform(SDL_FRect &dst) noexcept;
	};
} // namespace tudov
