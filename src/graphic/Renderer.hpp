#pragma once

#include "mod/LuaAPI.hpp"
#include "program/Context.hpp"
#include "resource/TextureManager.hpp"
#include "sol/table.hpp"
#include "util/Defs.hpp"
#include "util/Log.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace tudov
{
	struct IWindow;
	class Window;
	class RenderTarget;

	struct IRenderer
	{
		virtual ~IRenderer() noexcept = default;

		virtual IWindow &GetWindow() noexcept = 0;

		virtual bool DestroyTexture(const std::shared_ptr<Texture> &texture) noexcept = 0;
		virtual std::shared_ptr<Texture> GetRenderTexture() noexcept = 0;
		virtual void SetRenderTexture(const std::shared_ptr<Texture> &texture = nullptr) noexcept = 0;
		virtual void DrawTexture(const std::shared_ptr<Texture> &texture, const SDL_FRect &dst, const SDL_FRect &src, std::float_t z) noexcept = 0;

		virtual void Clear(const SDL_Color &color) noexcept = 0;
		virtual void Render() noexcept = 0;
	};

	class Renderer : public IRenderer, public IContextProvider, public ILuaAPIProvider
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
		static ILuaAPI::TInstallation rendererLuaAPIInstallation;

	  private:
		Window &_window;
		std::shared_ptr<Log> _log;
		SDL_Renderer *_sdlRenderer;
		// Manage image textures.
		TextureManager _textureManager;
		std::unordered_map<SDL_Texture *, std::shared_ptr<Texture>> _heldTextures;
		std::vector<RenderCommand> _renderCommands;

	  private:
		std::shared_ptr<Texture> GetOrCreateImageTexture(ImageID imageID) noexcept;

		void LuaDrawTexture(const sol::table &args);
		std::shared_ptr<RenderTarget> LuaNewRenderTarget(const sol::object &width = sol::nil, const sol::object &height = sol::nil);
		void LuaClear(std::uint32_t color) noexcept;

	  public:
		explicit Renderer(Window &window) noexcept;
		~Renderer() noexcept = default;

		void Initialize() noexcept;

		void ProvideLuaAPI(ILuaAPI &luaAPI) noexcept override;

		IWindow &GetWindow() noexcept override;
		Context &GetContext() noexcept override;

		std::weak_ptr<Texture> CreateTexture(std::int32_t width, std::int32_t height, SDL_PixelFormat format, SDL_TextureAccess access) noexcept;
		std::weak_ptr<Texture> CreateTexture(Image &image);
		bool DestroyTexture(const std::shared_ptr<Texture> &texture) noexcept override;
		std::shared_ptr<Texture> GetRenderTexture() noexcept override;
		void SetRenderTexture(const std::shared_ptr<Texture> &texture = nullptr) noexcept override;
		void DrawTexture(const std::shared_ptr<Texture> &texture, const SDL_FRect &dst, const SDL_FRect &src, std::float_t z) noexcept override;

		void Clear(const SDL_Color &color) noexcept override;
		void Render() noexcept override;

		SDL_Renderer *GetSDLRendererHandle() noexcept;

		inline const SDL_Renderer *GetSDLRendererHandle() const noexcept
		{
			return const_cast<Renderer *>(this)->GetSDLRendererHandle();
		}
	};
} // namespace tudov
