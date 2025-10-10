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

#include "graphic/BlendMode.hpp"
#include "graphic/RenderArgs.hpp"
#include "graphic/RenderBuffer.hpp"
#include "graphic/RenderTarget.hpp"
#include "graphic/VSyncMode.hpp"
#include "mod/ScriptEngine.hpp"
#include "program/Engine.hpp"
#include "program/Window.hpp"
#include "resource/GlobalResourcesCollection.hpp"
#include "resource/ImageResources.hpp"
#include "system/LogMicros.hpp"
#include "util/Utils.hpp"

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "sol/forward.hpp"
#include "sol/table.hpp"

#include <cmath>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <vector>

using namespace tudov;

Renderer::Renderer(Window &window) noexcept
    : _window(window),
      _log(Log::Get("Renderer")),
      _sdlRenderer(nullptr),
      _sdlTextureMain(nullptr),
      _sdlTextureBackground(nullptr)
{
	if (_sdlRenderer != nullptr)
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
	if (_sdlRenderer != nullptr) [[likely]]
	{
		auto [width, height] = _window.GetSize();
		_sdlTextureMain = SDL_CreateTexture(_sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
		_sdlTextureBackground = SDL_CreateTexture(_sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	}
	else [[unlikely]]
	{
		TE_ERROR("Failed to create SDL3 Renderer");
	}

	_sdlTTFTextEngine = TTF_CreateRendererTextEngine(_sdlRenderer);
	if (_sdlTTFTextEngine == nullptr) [[unlikely]]
	{
		TE_ERROR("Failed to create SDL3_TTF Text Engine");
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

std::shared_ptr<RenderBuffer> Renderer::NewRenderBuffer() noexcept
{
	auto &&renderBuffer = std::make_shared<RenderBuffer>(*this);
	return renderBuffer;
}

std::shared_ptr<Texture> Renderer::ExtractTexture(sol::table args) noexcept
{
	std::shared_ptr<Texture> texture;

	auto t = args["texture"];
	if (t.is<std::double_t>())
	{
		texture = GetOrCreateImageTexture(static_cast<ImageID>(t.get<std::double_t>()));
	}
	else if (t.is<std::shared_ptr<Image>>())
	{
		std::string_view filePath = t.get<std::shared_ptr<Image>>()->GetFilePath();
		ImageID imageID = GetGlobalResourcesCollection().GetImageResources().GetResourceID(filePath);
		texture = GetOrCreateImageTexture(imageID);
	}
	else if (t.is<std::shared_ptr<RenderTarget>>())
	{
		texture = t.get<std::shared_ptr<RenderTarget>>()->_texture;
	}
	else
	{
		texture = nullptr;
	}

	return texture;
}

std::shared_ptr<Texture> Renderer::GetOrCreateImageTexture(ImageID imageID)
{
	TextureID textureID;
	if (auto it = _imageTextureMap.find(imageID); it != _imageTextureMap.end()) [[likely]]
	{
		textureID = it->second;
	}
	else [[unlikely]]
	{
		auto &&imageResources = _window.GetGlobalResourcesCollection().GetImageResources();
		auto &&image = imageResources.GetResource(imageID);
		if (!image)
		{
			image = imageResources.GetResource(imageResources.GetResourceID("App/GFX/Placeholder.png"));
		}

		std::string_view path = imageResources.GetResourcePath(imageID);

		auto [texture, id] = _textureManager.LoadTexture(*this);
		texture->Initialize(*image);
		textureID = id;

		_imageTextureMap[imageID] = textureID;
	}

	return _textureManager.GetTexture(textureID);
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

void Renderer::ApplyTransform(SDL_FRect &rect) noexcept
{
	if (_renderTargets.empty())
	{
		return;
	}

	auto &renderTarget = _renderTargets.top();
	std::float_t x = renderTarget->_cameraX;
	std::float_t y = renderTarget->_cameraY;
	std::float_t scaleX = renderTarget->_cameraScaleX;
	std::float_t scaleY = renderTarget->_cameraScaleY;

	rect.x = ((rect.x - x) * scaleX) + _window.GetWidth() * 0.5f;
	rect.y = ((rect.y - y) * scaleY) + _window.GetHeight() * 0.5f;
	rect.w = (rect.w * scaleX);
	rect.h = (rect.h * scaleY);
}

void Renderer::DrawRect(Texture *texture, const SDL_FRect &dst, const SDL_FRect *src, Color color)
{
	SDL_FRect dst_{dst};
	ApplyTransform(dst_);

	SDL_Texture *sdlTexture = texture ? texture->GetSDLTextureHandle() : nullptr;
	if (sdlTexture == nullptr)
	{
		sdlTexture = GetOrCreateImageTexture(0)->GetSDLTextureHandle();
	}

	SDL_SetTextureColorMod(sdlTexture, color.r, color.g, color.b);
	SDL_SetTextureAlphaMod(sdlTexture, color.a);

	bool success = SDL_RenderTexture(_sdlRenderer, sdlTexture, src, &dst_);

	if (!success) [[unlikely]]
	{
		throw std::runtime_error(SDL_GetError());
	}
}

void Renderer::DrawDebugText(std::float_t x, std::float_t y, std::string_view text, Color color)
{
	// TODO
}

SDL_FRect Renderer::DrawText(DrawTextArgs *args)
{
	const std::shared_ptr<Font> &font = GetGlobalResourcesCollection().GetFontResources().GetResource(args->font);
	if (font == nullptr) [[unlikely]]
	{
		throw std::runtime_error("Invalid font");
	}

	auto [sdlTexture, width, height] = font->GetTextInfo(_sdlRenderer, args->characterSize, args->text, args->maxWidth);

	std::float_t w = static_cast<std::float_t>(width) * args->scale;
	std::float_t h = static_cast<std::float_t>(height) * args->scale;
	SDL_FRect dst = {
	    args->x - w * args->alignX,
	    args->y - h * args->alignY,
	    w,
	    h,
	};
	ApplyTransform(dst);

	if (args->backgroundColor.a != 0)
	{
		SDL_Texture *backgroundSDLTexture = GetOrCreateImageTexture(0)->GetSDLTextureHandle();
		SDL_SetTextureColorMod(backgroundSDLTexture, args->backgroundColor.r, args->backgroundColor.g, args->backgroundColor.b);
		SDL_SetTextureAlphaMod(backgroundSDLTexture, args->backgroundColor.a);
		SDL_RenderTexture(_sdlRenderer, backgroundSDLTexture, nullptr, &dst);
	}

	if (args->shadow != 0 && args->shadowColor.a != 0)
	{
		std::float_t offset = args->shadow * args->scale;

		dst.x += offset;
		dst.y += offset;

		SDL_SetTextureColorMod(sdlTexture, args->shadowColor.r, args->shadowColor.g, args->shadowColor.b);
		SDL_SetTextureAlphaMod(sdlTexture, args->shadowColor.a);
		SDL_RenderTexture(_sdlRenderer, sdlTexture, nullptr, &dst);

		dst.x -= offset;
		dst.y -= offset;
	}

	SDL_SetTextureColorMod(sdlTexture, args->color.r, args->color.g, args->color.b);
	SDL_SetTextureAlphaMod(sdlTexture, args->color.a);
	SDL_RenderTexture(_sdlRenderer, sdlTexture, nullptr, &dst);

	return {
	    args->x,
	    args->y,
	    w,
	    h,
	};
}

SDL_FRect Renderer::DrawRichText(DrawTextArgs *args)
{
	// TODO
	return {};
}

void Renderer::DrawVertices(Texture *texture, const std::vector<Vertex> &vertices)
{
	if (vertices.empty())
	{
		return;
	}

	bool success = SDL_RenderGeometry(
	    _sdlRenderer,
	    texture ? texture->GetSDLTextureHandle() : nullptr,
	    vertices.data(),
	    vertices.size(),
	    nullptr,
	    0);

	if (!success) [[unlikely]]
	{
		throw std::runtime_error(std::format("Error draw vertices: {}", SDL_GetError()));
	}
}

void Renderer::DrawVertices(Texture *texture, const std::vector<Vertex> &vertices, const std::vector<std::int32_t> &indices)
{
	if (vertices.empty())
	{
		return;
	}

	bool success = SDL_RenderGeometry(
	    _sdlRenderer,
	    texture ? texture->GetSDLTextureHandle() : nullptr,
	    vertices.data(),
	    vertices.size(),
	    indices.data(),
	    indices.size());

	if (!success) [[unlikely]]
	{
		throw std::runtime_error(std::format("Error draw vertices: {}", SDL_GetError()));
	}
}

void Renderer::LuaDrawRect(DrawRectArgs *args) noexcept
{
	try
	{
		if (args == nullptr) [[unlikely]]
		{
			GetScriptEngine().ThrowError("'Args' cannot be a null pointer");
		}

		Texture *texture;
		if (args->_texture.has_value()) [[likely]]
		{
			texture = args->_texture->get();
		}
		else [[unlikely]]
		{
			auto texture1 = args->texture;
			std::shared_ptr<Texture> texture2 = nullptr;

			if (texture1.is<std::double_t>())
			{
				texture2 = GetOrCreateImageTexture(static_cast<ImageID>(texture1.as<std::double_t>()));
			}
			else if (texture1.is<std::shared_ptr<Image>>())
			{
				std::string_view filePath = texture1.as<std::shared_ptr<Image>>()->GetFilePath();
				ImageID imageID = GetGlobalResourcesCollection().GetImageResources().GetResourceID(filePath);
				texture2 = GetOrCreateImageTexture(imageID);
			}
			else if (texture1.is<std::shared_ptr<RenderTarget>>())
			{
				texture2 = texture1.as<std::shared_ptr<RenderTarget>>()->GetTexture();
			}
			else
			{
				texture2 = nullptr;
			}

			args->_texture = texture2;
			texture = texture2.get();
		}

		SDL_FRect rectDst{
		    args->destination.x,
		    args->destination.y,
		    args->destination.w,
		    args->destination.h,
		};

		if (args->source.has_value())
		{
			auto value = args->source.value();
			SDL_FRect rectSrc{
			    value.x,
			    value.y,
			    value.w,
			    value.h,
			};

			DrawRect(texture, rectDst, &rectSrc, args->color);
		}
		else
		{
			DrawRect(texture, rectDst, nullptr, args->color);
		}
	}
	catch (std::exception &e)
	{
		GetScriptEngine().ThrowError("C++ exception in `{}`: {}", TE_NAMEOF(Renderer::DrawRect), e.what());
	}
}

void Renderer::LuaDrawDebugText(std::double_t x, std::double_t y, sol::string_view text) noexcept
{
	try
	{
		DrawDebugText(x, y, text);
	}
	catch (std::exception &e)
	{
		GetScriptEngine().ThrowError("C++ exception in `Renderer::DrawDebugText`: {}", e.what());
	}
}

std::tuple<std::float_t, std::float_t, std::float_t, std::float_t> Renderer::LuaDrawText(sol::object args) noexcept
{
	try
	{
		if (!args.is<DrawTextArgs>()) [[unlikely]]
		{
			GetScriptEngine().ThrowError("Bad argument to #1 'args': userdata `DrawTextArgs` expected got {}", GetLuaTypeStringView(args.get_type()));
		}

		auto &args_ = args.as<DrawTextArgs>();
		SDL_FRect rect = DrawText(&args_);
		return std::make_tuple(rect.x, rect.y, rect.w, rect.h);
	}
	catch (std::exception &e)
	{
		GetScriptEngine().ThrowError("C++ exception in `Renderer::DrawDebugText`: {}", e.what());
	}
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

EBlendMode Renderer::GetBlendMode(Texture *texture) noexcept
{
	SDL_BlendMode sdlBlendMode;
	SDL_GetTextureBlendMode(texture != nullptr ? texture->GetSDLTextureHandle() : nullptr, &sdlBlendMode);
	return static_cast<EBlendMode>(sdlBlendMode);
}

void Renderer::SetBlendMode(Texture *texture, EBlendMode blendMode) noexcept
{
	SDL_SetTextureBlendMode(texture != nullptr ? texture->GetSDLTextureHandle() : nullptr, static_cast<SDL_BlendMode>(blendMode));
}

void Renderer::Clear() noexcept
{
	SDL_SetRenderDrawColor(_sdlRenderer, 0, 0, 0, 0);
	SDL_RenderClear(_sdlRenderer);
}

void Renderer::Reset() noexcept
{
}

void Renderer::LuaClear() noexcept
{
	Clear();
}

void Renderer::Begin() noexcept
{
	{
		auto loadingState = GetEngine().GetLoadingState();
		_background = loadingState == Engine::ELoadingState::Pending || loadingState == Engine::ELoadingState::InProgress;
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
			SDL_DestroyTexture(_sdlTextureMain);
			SDL_DestroyTexture(_sdlTextureBackground);

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
