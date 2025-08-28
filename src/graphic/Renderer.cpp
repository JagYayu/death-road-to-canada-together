/**
 * @file graphic/Renderer.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "graphic/Renderer.hpp"

#include "graphic/RenderTarget.hpp"
#include "graphic/VSyncMode.hpp"
#include "program/Engine.hpp"
#include "program/Window.hpp"
#include "resource/GlobalResourcesCollection.hpp"
#include "resource/ImageResources.hpp"
#include "system/LogMicros.hpp"

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "sol/forward.hpp"
#include "sol/table.hpp"

#include <cmath>
#include <memory>
#include <stdexcept>
#include <tuple>

using namespace tudov;

Renderer::Renderer(Window &window) noexcept
    : _window(window),
      _log(Log::Get("Renderer")),
      _sdlRenderer(nullptr),
      _sdlTextureMain(nullptr),
      _sdlTextureBackground(nullptr)
{
	if (_sdlRenderer)
	{
		DeinitializeRenderer();
	}
}

Log &Renderer::GetLog() noexcept
{
	return *_log;
}

void Renderer::InitializeRenderer() noexcept
{
	if (_sdlRenderer) [[unlikely]]
	{
		TE_WARN("{}", "Already initialized renderer");
		return;
	}

	_sdlRenderer = SDL_CreateRenderer(_window.GetSDLWindowHandle(), nullptr);
	if (_sdlRenderer != nullptr)
	{
		auto [width, height] = _window.GetSize();
		_sdlTextureMain = SDL_CreateTexture(_sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
		_sdlTextureBackground = SDL_CreateTexture(_sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	}
	else
	{
		TE_ERROR("{}", "Failed to create SDL3 Renderer");
	}
}

void Renderer::DeinitializeRenderer() noexcept
{
	if (_sdlRenderer) [[likely]]
	{
		SDL_DestroyRenderer(_sdlRenderer);
		_sdlRenderer = nullptr;
	}
	else [[unlikely]]
	{
		_log->Error("Failed to deinitialize renderer");
	}
}

IWindow &Renderer::GetWindow() noexcept
{
	return _window;
}

Context &Renderer::GetContext() noexcept
{
	return _window.GetContext();
}

std::shared_ptr<RenderTarget> Renderer::NewRenderTarget(std::int32_t width, std::int32_t height) noexcept
{
	auto &&renderTarget = std::make_shared<RenderTarget>(*this, width, height);
	return renderTarget;
}

std::shared_ptr<Texture> Renderer::GetOrCreateImageTexture(ImageID imageID)
{
	TextureID textureID;
	if (auto it = _imageTextureMap.find(imageID); it != _imageTextureMap.end()) [[likely]]
	{
		textureID = it->second;
	}
	else
	{
		auto &&imageResources = _window.GetGlobalResourcesCollection().GetImageResources();
		auto &&image = imageResources.GetResource(imageID);
		if (!image)
		{
			image = imageResources.GetResource(imageResources.GetResourceID("app/gfx/PlaceHolder.png"));
		}

		std::string_view path = imageResources.GetResourcePath(imageID);

		auto [texture, id] = _textureManager.LoadTexture(*this);
		texture->Initialize(*image);
		textureID = id;

		_imageTextureMap[imageID] = textureID;
	}

	return _textureManager.GetTexture(textureID);
}

bool Renderer::ReleaseTexture(const std::shared_ptr<Texture> &texture) noexcept
{
	return _heldTextures.erase(texture.get()->GetSDLTextureHandle());
}

std::shared_ptr<Texture> Renderer::GetRenderTexture() noexcept
{
	if (auto &&sdlTexture = SDL_GetRenderTarget(_sdlRenderer); sdlTexture != nullptr)
	{
		auto it = _heldTextures.find(sdlTexture);
		if (it != _heldTextures.end())
		{
			return it->second;
		}
	}
	return nullptr;
}

void Renderer::SetRenderTexture(const std::shared_ptr<Texture> &texture) noexcept
{
	if (texture == nullptr)
	{
		SDLSetRenderTarget(_sdlTextureMain);
	}
	else if (auto &&sdlTexture = texture->GetSDLTextureHandle(); sdlTexture != nullptr)
	{
		SDLSetRenderTarget(sdlTexture);
	}
}

EVSyncMode Renderer::GetVSync() noexcept
{
	std::int32_t vsync;
	SDL_GetRenderVSync(_sdlRenderer, &vsync);
	return EVSyncMode(vsync);
}

bool Renderer::SetVSync(EVSyncMode value) noexcept
{
	return SDL_SetRenderVSync(_sdlRenderer, std::int32_t(value));
}

SDL_FRect Renderer::ApplyTransform(const SDL_FRect &rect) noexcept
{
	if (_renderTargets.empty())
	{
		return rect;
	}

	auto &renderTarget = _renderTargets.top();
	std::float_t x = renderTarget->_cameraX;
	std::float_t y = renderTarget->_cameraY;
	std::float_t scaleX = renderTarget->_cameraScaleX;
	std::float_t scaleY = renderTarget->_cameraScaleY;

	auto [w, h] = renderTarget->GetSize();

	auto dst = rect;

	dst.x = ((rect.x - x) * scaleX) + _window.GetWidth() * 0.5f;
	dst.y = ((rect.y - y) * scaleY) + _window.GetHeight() * 0.5f;
	dst.w = (rect.w * scaleX);
	dst.h = (rect.h * scaleY);

	return dst;
}

void Renderer::Draw(const std::shared_ptr<Texture> &texture, const SDL_FRect &dst, const SDL_FRect &src)
{
	auto tDst = ApplyTransform(dst);
	if (!SDL_RenderTexture(_sdlRenderer, texture->GetSDLTextureHandle(), &src, &tDst)) [[unlikely]]
	{
		throw std::runtime_error(SDL_GetError());
	}
}

void Renderer::LuaDraw(sol::table args)
{
	try
	{
		auto &&texture = LuaDrawExtractTexture(args);
		if (texture == nullptr) [[unlikely]]
		{
			throw std::runtime_error("Texture was not found");
		}

		if (!args["destination"].is<sol::table>()) [[unlikely]]
		{
			throw std::runtime_error("Failed to draw rect: missing parameter 'destination'");
		}

		SDL_FRect rectDst;
		{
			auto t = args["destination"].get<sol::table>();
			rectDst.x = t.get_or<std::float_t>(1, 0);
			rectDst.y = t.get_or<std::float_t>(2, 0);
			rectDst.w = t.get_or<std::float_t>(3, 0);
			rectDst.h = t.get_or<std::float_t>(4, 0);
		}

		SDL_FRect rectSrc;
		if (texture != nullptr)
		{
			auto source = args["source"].get<sol::object>();
			auto [tw, th] = texture->GetSize();
			if (source.valid())
			{
				auto t = source.as<sol::table>();
				rectSrc.x = t.get_or<std::float_t>(1, 0);
				rectSrc.y = t.get_or<std::float_t>(2, 0);
				rectSrc.w = t.get_or<std::float_t>(3, tw);
				rectSrc.h = t.get_or<std::float_t>(4, th);
			}
			else
			{
				rectSrc.x = 0;
				rectSrc.y = 0;
				rectSrc.w = tw;
				rectSrc.h = th;
			}
		}

		Draw(texture, rectDst, rectSrc);
	}
	catch (std::exception &e)
	{
		GetScriptEngine().ThrowError("C++ exception in `Renderer::DrawImage`: {}", e.what());
	}
}

std::shared_ptr<Texture> Renderer::LuaDrawExtractTexture(sol::table args) noexcept
{
	{
		auto &&renderTarget = args.get_or("renderTarget", static_cast<RenderTarget *>(nullptr));
		if (renderTarget != nullptr)
		{
			return renderTarget->_texture;
		}
	}

	{
		auto imageID = args.get_or<ImageID>("image", 0);
		auto &&texture = GetOrCreateImageTexture(imageID);
		if (texture != nullptr)
		{
			return texture;
		}
	}

	return nullptr;
}

std::shared_ptr<RenderTarget> Renderer::LuaNewRenderTarget(sol::object width, sol::object height)
{
	return std::make_shared<RenderTarget>(*this,
	                                      width.is<double_t>() ? int32_t(width.as<double_t>()) : _window.GetWidth(),
	                                      height.is<double_t>() ? int32_t(height.as<double_t>()) : _window.GetHeight());
}

void Renderer::BeginTarget(const std::shared_ptr<RenderTarget> &renderTarget) noexcept
{
	if (renderTarget == nullptr) [[unlikely]]
	{
		return;
	}

	_renderTargets.push(renderTarget);

	auto &&texture = renderTarget->_texture;
	if (texture != nullptr)
	{
		SetRenderTexture(texture);
	}
}

void Renderer::LuaBeginTarget(sol::object renderTarget) noexcept
{
	try
	{
		if (renderTarget.is<std::shared_ptr<RenderTarget>>())
		{
			BeginTarget(renderTarget.as<std::shared_ptr<RenderTarget>>());
		}
	}
	catch (std::exception &e)
	{
		TE_ASSERT(false, "UNHANDLED ERROR");
	}
}

std::shared_ptr<RenderTarget> Renderer::EndTarget() noexcept
{
	if (_renderTargets.empty())
	{
		return nullptr;
	}

	std::shared_ptr<RenderTarget> &renderTarget = _renderTargets.top();
	_renderTargets.pop();

	SetRenderTexture(_renderTargets.empty() ? nullptr : _renderTargets.top()->_texture);

	return renderTarget;
}

std::shared_ptr<RenderTarget> Renderer::LuaEndTarget() noexcept
{
	try
	{
		return EndTarget();
	}
	catch (std::exception &e)
	{
		TE_ASSERT(false, "UNHANDLED ERROR");
		return nullptr;
	}
}

void Renderer::Clear(const SDL_Color &color) noexcept
{
	SDL_SetRenderDrawColor(_sdlRenderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(_sdlRenderer);
}

void Renderer::Reset() noexcept
{
}

void Renderer::LuaClear(std::uint32_t color) noexcept
{
	// std::uint8_t r = (color >> 24) & 0xFF;
	// std::uint8_t g = (color >> 16) & 0xFF;
	// std::uint8_t b = (color >> 8) & 0xFF;
	// std::uint8_t a = (color >> 0) & 0xFF;
	// Clear(SDL_Color{r, g, b, a});
	Clear(SDL_Color(0, 0, 0, 0));
}

void Renderer::Render() noexcept
{
	// if (_renderTargets.empty())
	// {
	// 	RenderImpl(_renderCommands);
	// }
	// else
	// {
	// 	RenderImpl(_renderTargets.top()->_renderCommands);
	// }
}

void Renderer::Begin() noexcept
{
	{
		auto state = GetEngine().GetLoadingState().load();
		_background = state == Engine::ELoadingState::Pending || state == Engine::ELoadingState::InProgress;
	}

	SDL_SetRenderTarget(_sdlRenderer, nullptr);
	SDL_SetRenderDrawColor(_sdlRenderer, 0, 0, 0, 255);
	SDL_RenderClear(_sdlRenderer);
	SDL_SetRenderTarget(_sdlRenderer, _sdlTextureMain);
	SDL_RenderClear(_sdlRenderer);

	if (_background)
	{
		SDL_RenderTexture(_sdlRenderer, _sdlTextureBackground, nullptr, nullptr);
	}
}

void Renderer::End() noexcept
{
	SDLSetRenderTarget(nullptr);
	SDLRenderTexture(_sdlTextureMain, nullptr, nullptr);

	// Resize textures
	{
		auto [w1, h1] = _window.GetSize();
		std::float_t w2, h2;
		SDL_GetTextureSize(_sdlTextureMain, &w2, &h2);

		if (static_cast<std::float_t>(w1) != w2 || static_cast<std::float_t>(h1) != h2) [[unlikely]]
		{
			_sdlTextureMain = SDL_CreateTexture(_sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w1, h1);
			_sdlTextureBackground = SDL_CreateTexture(_sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w1, h1);
		}
	}

	if (!_background)
	{
		SDLSetRenderTarget(_sdlTextureBackground);
		SDLSetRenderDrawColor(0, 0, 0, 255);
		SDLRenderClear();
		SDLRenderTexture(_sdlTextureMain, nullptr, nullptr);
	}

	SDLRenderPresent();

	if (!_renderTargets.empty())
	{
		TE_WARN("{}", "Render target stack is not empty!");
		do
		{
			_renderTargets.pop();
		} while (!_renderTargets.empty());
	}
}

SDL_Renderer *Renderer::GetSDLRendererHandle() noexcept
{
	return _sdlRenderer;
}

std::tuple<std::float_t, std::float_t> Renderer::LuaGetTargetSize(const std::shared_ptr<RenderTarget> &renderTarget) noexcept
{
	return renderTarget->_texture->GetSize();
}

void Renderer::TryLogSDLError(bool value) noexcept
{
	if (!value) [[unlikely]]
	{
		TE_WARN("{}", SDL_GetError());
	}
}

void Renderer::SDLRenderClear() noexcept
{
	TryLogSDLError(SDL_RenderClear(_sdlRenderer));
}

void Renderer::SDLRenderTexture(SDL_Texture *sdlTexture, const SDL_FRect *srcRect, const SDL_FRect *dstRect) noexcept
{
	TryLogSDLError(SDL_RenderTexture(_sdlRenderer, sdlTexture, srcRect, dstRect));
}

void Renderer::SDLSetRenderDrawColor(std::float_t r, std::float_t g, std::float_t b, std::float_t a) noexcept
{
	TryLogSDLError(SDL_SetRenderDrawColor(_sdlRenderer, r, g, b, a));
}

void Renderer::SDLRenderPresent() noexcept
{
	TryLogSDLError(SDL_RenderPresent(_sdlRenderer));
}

void Renderer::SDLSetRenderTarget(SDL_Texture *sdlTexture) noexcept
{
	TryLogSDLError(SDL_SetRenderTarget(_sdlRenderer, sdlTexture));
}
