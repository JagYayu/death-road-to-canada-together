#pragma once

#include "RenderCommand.hpp"
#include "SDL3/SDL_rect.h"
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

namespace tudov
{
	struct IWindow;

	struct IRenderer
	{
		virtual ~IRenderer() noexcept = default;

		virtual IWindow &GetWindow() noexcept = 0;

		virtual bool ReleaseTexture(const std::shared_ptr<Texture> &texture) noexcept = 0;
		virtual std::shared_ptr<Texture> GetRenderTexture() noexcept = 0;
		// virtual void PushRenderTarget(const std::shared_ptr<RenderTarget> &renderTarget) noexcept = 0;
		// virtual bool PopRenderTexture(const SDL_FRect &destination, const SDL_FRect &source) noexcept = 0;
		virtual void SetRenderTexture(const std::shared_ptr<Texture> &texture = nullptr) noexcept = 0;
		virtual void DrawTexture(const std::shared_ptr<Texture> &texture, const SDL_FRect &dst, const SDL_FRect &src, std::float_t z) noexcept = 0;

		virtual void Clear(const SDL_Color &color) noexcept = 0;
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

	  private:
		std::shared_ptr<Texture> GetOrCreateImageTexture(ImageID imageID);

		void LuaBeginTarget(const std::shared_ptr<RenderTarget> &renderTarget) noexcept;
		bool LuaEndTarget(const sol::table &source, const sol::table &destination) noexcept;
		void LuaDraw(const sol::table &args);
		std::shared_ptr<RenderTarget> LuaNewRenderTarget(const sol::object &width = sol::nil, const sol::object &height = sol::nil);
		void LuaClear(std::uint32_t color) noexcept;

	  public:
		explicit Renderer(Window &window) noexcept;
		~Renderer() noexcept = default;

		void Initialize() noexcept;

		// void ProvideLuaAPI(ILuaAPI &luaAPI) noexcept override;

		IWindow &GetWindow() noexcept override;
		Context &GetContext() noexcept override;

		std::shared_ptr<RenderTarget> NewRenderTarget(std::int32_t width, std::int32_t height) noexcept;
		void BeginTarget(const std::shared_ptr<RenderTarget> &renderTarget) noexcept;
		bool EndTarget(const SDL_FRect &source, const SDL_FRect &destination) noexcept;

		// std::weak_ptr<Texture> CreateTexture(std::int32_t width, std::int32_t height, SDL_PixelFormat format, SDL_TextureAccess access) noexcept;
		// std::weak_ptr<Texture> CreateTexture(Image &image);
		bool ReleaseTexture(const std::shared_ptr<Texture> &texture) noexcept override;
		std::shared_ptr<Texture> GetRenderTexture() noexcept override;
		void SetRenderTexture(const std::shared_ptr<Texture> &texture = nullptr) noexcept override;
		void DrawTexture(const std::shared_ptr<Texture> &texture, const SDL_FRect &dst, const SDL_FRect &src, std::float_t z) noexcept override;

		void Clear(const SDL_Color &color) noexcept override;
		void Render() noexcept override;
		void Begin() noexcept override;
		void End() noexcept override;

		SDL_Renderer *GetSDLRendererHandle() noexcept;

		inline const SDL_Renderer *GetSDLRendererHandle() const noexcept
		{
			return const_cast<Renderer *>(this)->GetSDLRendererHandle();
		}

	  private:
		void RenderImpl(std::vector<RenderCommand> &renderCommands) noexcept;

		// void PushRenderTarget(std::shared_ptr<RenderTarget> renderTarget) noexcept;
		// bool PopRenderTexture(const SDL_FRect &destination, const SDL_FRect &source) noexcept;
	};
} // namespace tudov
