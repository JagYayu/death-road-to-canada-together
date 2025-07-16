#include "Renderer.hpp"

#include "RenderTarget.hpp"
#include "program/Window.hpp"
#include "resource/ImageManager.hpp"

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "sol/table.hpp"

#include <cmath>
#include <memory>
#include <utility>

using namespace tudov;

Renderer::Renderer(Window &window) noexcept
    : _window(window),
      _log(Log::Get("Renderer")),
      _sdlRenderer(nullptr)
{
}

void Renderer::Initialize() noexcept
{
	_sdlRenderer = SDL_CreateRenderer(_window.GetSDLWindowHandle(), nullptr);
	if (!_sdlRenderer)
	{
		_log->Error("Failed to create SDL3 Renderer");
	}
}

// ILuaAPI::TInstallation Renderer::rendererLuaAPIInstallation = [](sol::state &lua)
// {
// 	lua.new_usertype<Renderer>("tudov_Renderer",
// 	                           "clear", &Renderer::LuaClear,
// 	                           "render", &Renderer::Render,
// 	                           "drawRect", &Renderer::LuaDrawTexture,
// 	                           "newRenderTarget", &Renderer::LuaNewRenderTarget,
// 	                           "renderTexture", &Renderer::DrawTexture);

// 	lua.new_usertype<RenderTarget>("tudov_RenderTarget",
// 	                               "beginTarget", &RenderTarget::LuaBeginTarget,
// 	                               "endTarget", &RenderTarget::LuaEndTarget);
// };

// void Renderer::ProvideLuaAPI(ILuaAPI &luaAPI) noexcept
// {
// 	luaAPI.RegisterInstallation("tudov_Renderer", rendererLuaAPIInstallation);
// }

IWindow &Renderer::GetWindow() noexcept
{
	return _window;
}

Context &Renderer::GetContext() noexcept
{
	return _window.GetContext();
}

std::shared_ptr<Texture> Renderer::GetOrCreateImageTexture(ImageID imageID) noexcept
{
	auto &&texture = _textureManager.GetResource(imageID);
	if (!texture) [[unlikely]]
	{
		auto &&imageManager = _window.GetImageManager();
		auto &&image = imageManager.GetResource(imageID);
		if (!image)
		{
			return nullptr;
		}

		auto &&path = imageManager.GetResourcePath(imageID);
		auto textureID = _textureManager.Load(path, *this);
		assert(textureID);
		texture = _textureManager.GetResource(textureID);
		texture->Initialize(*image);
	}

	return texture;
}

std::weak_ptr<Texture> Renderer::CreateTexture(std::int32_t width, std::int32_t height, SDL_PixelFormat format, SDL_TextureAccess access) noexcept
{
	auto &&texture = std::make_shared<Texture>(*this);
	texture->Initialize(width, height, format, access);
	_heldTextures[texture.get()->GetSDLTextureHandle()] = texture;
	return texture;
}

std::weak_ptr<Texture> Renderer::CreateTexture(Image &image)
{
	auto &&texture = std::make_shared<Texture>(*this);
	texture->Initialize(image);
	_heldTextures[texture.get()->GetSDLTextureHandle()] = texture;
	return texture;
}

bool Renderer::DestroyTexture(const std::shared_ptr<Texture> &texture) noexcept
{
	return _heldTextures.erase(texture.get()->GetSDLTextureHandle());
}

std::shared_ptr<Texture> Renderer::GetRenderTexture() noexcept
{
	if (auto &&sdlTexture = SDL_GetRenderTarget(_sdlRenderer); sdlTexture != nullptr)
	{
		auto &&it = _heldTextures.find(sdlTexture);
		if (it != _heldTextures.end())
		{
			return it->second;
		}
	}
	return nullptr;
}

void Renderer::SetRenderTexture(const std::shared_ptr<Texture> &texture) noexcept
{
	if (auto &&sdlTexture = SDL_GetRenderTarget(_sdlRenderer); sdlTexture != nullptr)
	{
		_heldTextures.erase(sdlTexture);
	}

	if (texture != nullptr)
	{
		SDL_SetRenderTarget(_sdlRenderer, texture->GetSDLTextureHandle());
		_heldTextures[texture.get()->GetSDLTextureHandle()] = texture;
	}
}

void Renderer::DrawTexture(const std::shared_ptr<Texture> &texture, const SDL_FRect &dst, const SDL_FRect &src, std::float_t z) noexcept
{
	DrawTextureCommand cmd{};

	cmd.tex = texture->GetSDLTextureHandle();
	cmd.dst = dst;
	cmd.src = src;
	cmd.ang = 0;
	cmd.nulls = DrawTextureCommand::Ori;

	_renderCommands.emplace_back(RenderCommand{
	    .drawTexture = cmd,
	    .type = RenderCommandType::DrawTexture,
	    .z = z,
	});
}

void Renderer::LuaDrawTexture(const sol::table &args)
{
	DrawTextureCommand cmd{};

	auto texture = GetOrCreateImageTexture(args.get_or<ImageID>("image", 0));
	if (!texture) [[unlikely]]
	{
		_log->Error("Failed to draw rect: invalid 'image'");
		return;
	}
	cmd.tex = texture->GetSDLTextureHandle();

	if (const sol::object &destination = args["destination"])
	{
		if (!destination.is<sol::table>()) [[unlikely]]
		{
			_log->Error("Failed to draw rect: invalid 'destination'");
			return;
		}

		sol::table t = destination.as<sol::table>();
		cmd.dst.x = t.get_or(1, 0);
		cmd.dst.y = t.get_or(2, 0);
		cmd.dst.w = t.get_or(3, 0);
		cmd.dst.h = t.get_or(4, 0);
	}

	if (const sol::object &source = args["source"]; source.is<sol::table>())
	{
		sol::table t = source.as<sol::table>();
		cmd.src.x = t.get_or(1, 0);
		cmd.src.y = t.get_or(2, 0);
		cmd.src.w = t.get_or(3, 0);
		cmd.src.h = t.get_or(4, 0);
	}
	else
	{
		cmd.nulls = DrawTextureCommand::Nulls(cmd.nulls | DrawTextureCommand::Src);
	}

	if (const sol::object &origin = args["origin"]; origin.is<sol::table>())
	{
		sol::table ori = origin.as<sol::table>();
		cmd.ori.x = ori.get_or(1, 0);
		cmd.ori.y = ori.get_or(2, 0);
	}
	else
	{
		cmd.nulls = DrawTextureCommand::Nulls(cmd.nulls | DrawTextureCommand::Ori);
	}

	cmd.ang = args.get_or("angle", 0.0f);

	_renderCommands.emplace_back(RenderCommand{
	    .drawTexture = cmd,
	    .type = RenderCommandType::DrawTexture,
	    .z = args.get_or("z", 0.0f),
	});
}

std::shared_ptr<RenderTarget> Renderer::LuaNewRenderTarget(const sol::object &width, const sol::object &height)
{
	return std::make_shared<RenderTarget>(*this,
	                                      width.is<double_t>() ? int32_t(width.as<double_t>()) : _window.GetWidth(),
	                                      height.is<double_t>() ? int32_t(height.as<double_t>()) : _window.GetHeight());
}

void Renderer::Clear(const SDL_Color &color) noexcept
{
	SDL_SetRenderDrawColor(_sdlRenderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(_sdlRenderer);
}

void Renderer::LuaClear(std::uint32_t color) noexcept
{
	std::uint8_t r = (color >> 24) & 0xFF;
	std::uint8_t g = (color >> 16) & 0xFF;
	std::uint8_t b = (color >> 8) & 0xFF;
	std::uint8_t a = (color >> 0) & 0xFF;
	Clear(SDL_Color{r, g, b, a});
}

void Renderer::Render() noexcept
{
	std::sort(_renderCommands.begin(), _renderCommands.end());

	for (auto &&command : _renderCommands)
	{
		switch (command.type)
		{
		case RenderCommandType::DrawTexture:
		{
			auto &args = command.drawTexture;
			SDL_FRect *src = (args.nulls & DrawTextureCommand::Src) ? nullptr : &args.src;
			SDL_FPoint *ori = (args.nulls & DrawTextureCommand::Ori) ? nullptr : &args.ori;
			SDL_RenderTextureRotated(_sdlRenderer, args.tex, src, &args.dst, args.ang, ori, SDL_FlipMode::SDL_FLIP_NONE);
			break;
		}
		case RenderCommandType::DrawRect:
		{
			break;
		}
		}
	}

	_renderCommands.clear();

	SDL_RenderPresent(_sdlRenderer);
}

SDL_Renderer *Renderer::GetSDLRendererHandle() noexcept
{
	return _sdlRenderer;
}
