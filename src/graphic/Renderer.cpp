#include "Renderer.h"
#include "Window.h"
#include "program/Engine.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_Surface.h"
#include "SDL3/SDL_render.h"
#include "sol/table.hpp"

#include <memory>

using namespace tudov;

Renderer::Renderer(Window &window) noexcept
    : window(window),
      _log(Log::Get("Renderer")),
      _sdlRenderer(nullptr)
{
}

void Renderer::Initialize() noexcept
{
	_sdlRenderer = SDL_CreateRenderer(window.GetSDLWindowHandle(), nullptr);
	if (!_sdlRenderer)
	{
		_log->Error("Failed to create SDL3 Renderer");
	}
}

std::shared_ptr<Texture> Renderer::GetOrCreateTexture(ImageID imageID) noexcept
{
	auto &&texture = _textureManager.GetResource(imageID);
	if (!texture) [[unlikely]]
	{
		auto &&image = window.engine.imageManager.GetResource(imageID);
		if (!image)
		{
			return nullptr;
		}

		auto &&path = window.engine.imageManager.GetResourcePath(imageID);
		auto textureID = _textureManager.Load(path, *this);
		assert(textureID);
		texture = _textureManager.GetResource(textureID);
		texture->Initialize(*image);
	}

	return texture;
}

void Renderer::SetRenderTarget(const std::shared_ptr<Texture> &texture) noexcept
{
	SDL_SetRenderTarget(_sdlRenderer, texture->GetSDLTextureHandle());
}

void Renderer::DrawTexture(const std::shared_ptr<Texture> &texture, const SDL_FRect &dst, const SDL_FRect &src, std::float_t z) noexcept
{
	// TODO
	// SDL_RenderTexture(_sdlRenderer, texture->GetSDLTextureHandle(), &src, &dst);
}

void Renderer::LuaDrawTexture(const sol::table &args)
{
	DrawTextureCommand cmd{};

	auto texture = GetOrCreateTexture(args.get_or<ImageID>("image", 0));
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
	// SDL_RenderTextureRotated(_sdlRenderer, texture->GetSDLTextureHandle(),
	//                          psrc, &dst, angle, pcenter, SDL_FLIP_NONE);
}

std::shared_ptr<Texture> Renderer::LuaNewRenderTexture(std::int32_t width, std::int32_t height)
{
	auto &&texture = std::make_shared<Texture>(*this);
	texture->Initialize(width, height, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET);
	return texture;
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

const SDL_Renderer *Renderer::GetSDLRendererHandle() const noexcept
{
	return _sdlRenderer;
}
