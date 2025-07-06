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
	auto &&texture = textureManager.GetResource(imageID);
	if (!texture) [[unlikely]]
	{
		auto &&image = window.engine.imageManager.GetResource(imageID);
		if (!image)
		{
			return nullptr;
		}

		auto &&path = window.engine.imageManager.GetResourcePath(imageID);
		auto textureID = textureManager.Load(path, *this);
		assert(textureID);
		texture = textureManager.GetResource(textureID);
		texture->Initialize(*image);
	}

	return texture;
}

void Renderer::SetRenderTarget(const std::shared_ptr<Texture> &texture) noexcept
{
	SDL_SetRenderTarget(_sdlRenderer, texture->GetSDLTextureHandle());
}

void Renderer::RenderTexture(const SDL_FRect &dst, const SDL_FRect &src, const std::shared_ptr<Texture> &texture) noexcept
{
	SDL_RenderTexture(_sdlRenderer, texture->GetSDLTextureHandle(), &src, &dst);
}

void Renderer::LuaDrawRect(const sol::table &args)
{
	auto texture = GetOrCreateTexture(args.get_or<ImageID>("image", 0));
	if (!texture) [[unlikely]]
	{
		_log->Error("Failed to draw rect: invalid 'image'");
		return;
	}

	SDL_FRect dst{};
	if (const sol::object &destination = args["destination"])
	{
		if (!destination.is<sol::table>()) [[unlikely]]
		{
			_log->Error("Failed to draw rect: invalid 'destination'");
			return;
		}

		sol::table t = destination.as<sol::table>();
		dst.x = t.get_or(1, 0);
		dst.y = t.get_or(2, 0);
		dst.w = t.get_or(3, 0);
		dst.h = t.get_or(4, 0);
	}

	SDL_FRect *psrc = nullptr;
	SDL_FRect src{};
	if (const sol::object &source = args["source"]; source.is<sol::table>())
	{
		sol::table t = source.as<sol::table>();
		src.x = t.get_or(1, 0);
		src.y = t.get_or(2, 0);
		src.w = t.get_or(3, 0);
		src.h = t.get_or(4, 0);
		psrc = &src;
	}

	SDL_FPoint *pcenter;
	SDL_FPoint center{};
	if (const sol::object &origin = args["origin"]; origin.is<sol::table>())
	{
		sol::table ori = origin.as<sol::table>();
		center.x = ori.get_or(1, 0);
		center.y = ori.get_or(2, 0);
		pcenter = &center;
	}

	std::float_t angle = args.get_or("angle", 0.0f);

	SDL_RenderTextureRotated(_sdlRenderer, texture->GetSDLTextureHandle(),
	                         psrc, &dst, angle, pcenter, SDL_FLIP_NONE);
}

std::shared_ptr<Texture> Renderer::LuaNewRenderTexture(std::int32_t width, std::int32_t height)
{
	auto &&texture = std::make_shared<Texture>(*this);
	texture->Initialize(width, height, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET);
	return texture;
}

void Renderer::RenderPresent() noexcept
{
	
	
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
