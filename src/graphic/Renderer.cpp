#include "graphic/Renderer.hpp"
#include "graphic/RenderTarget.hpp"
#include "graphic/VSyncMode.hpp"
#include "program/Engine.hpp"
#include "program/Window.hpp"
#include "resource/ImageResources.hpp"
#include "resource/GlobalResourcesCollection.hpp"

#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "sol/table.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <tuple>

using namespace tudov;

Renderer::Renderer(Window &window) noexcept
    : _window(window),
      _log(Log::Get("Renderer")),
      _sdlRenderer(nullptr),
      _sdlTextureMain(nullptr),
      _sdlTextureBackground(nullptr)
{
}

void Renderer::Initialize() noexcept
{
	_sdlRenderer = SDL_CreateRenderer(_window.GetSDLWindowHandle(), nullptr);
	if (!_sdlRenderer)
	{
		_log->Error("Failed to create SDL3 Renderer");
	}

	auto [width, height] = _window.GetSize();
	_sdlTextureMain = SDL_CreateTexture(_sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	_sdlTextureBackground = SDL_CreateTexture(_sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
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
	if (!imageID) [[unlikely]]
	{
		return nullptr;
	}

	TextureID textureID;
	if (auto &&it = _imageTextureMap.find(imageID); it != _imageTextureMap.end()) [[likely]]
	{
		textureID = it->second;
	}
	else [[unlikely]]
	{
		auto &&imageResources = _window.GetGlobalResourcesCollection().GetImageResources();
		auto &&image = imageResources.GetResource(imageID);
		if (!image)
		{
			return nullptr;
		}

		std::string_view path = imageResources.GetResourcePath(imageID);
		textureID = _textureManager.Load(path, *this);
		assert(textureID);
		_imageTextureMap[imageID] = textureID;

		_textureManager.GetResource(textureID)->Initialize(*image);
	}

	return _textureManager.GetResource(textureID);
}

bool Renderer::ReleaseTexture(const std::shared_ptr<Texture> &texture) noexcept
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
	if (texture == nullptr)
	{
		SDL_SetRenderTarget(_sdlRenderer, _sdlTextureMain);
	}
	else if (auto &&sdlTexture = texture->GetSDLTextureHandle(); sdlTexture != nullptr)
	{
		SDL_SetRenderTarget(_sdlRenderer, sdlTexture);
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

void Renderer::ApplyTransform(SDL_FRect &dst) noexcept
{
	std::float_t positionX = 0.0f;
	std::float_t positionY = 0.0f;
	std::float_t scaleX = 1.0f;
	std::float_t scaleY = 1.0f;
	std::float_t pivotX = 0.0f;
	std::float_t pivotY = 0.0f;

	if (!_renderTargets.empty())
	{
		auto &renderTarget = _renderTargets.top();
		scaleX = renderTarget->_scaleX;
		scaleY = renderTarget->_scaleY;
		positionX = renderTarget->_positionX;
		positionY = renderTarget->_positionY;

		auto [width, height] = renderTarget->GetSize();
		pivotX = width * 0.5f;
		pivotY = height * 0.5f;
	}

	dst.x -= pivotX;
	dst.y -= pivotY;

	dst.x *= scaleX;
	dst.y *= scaleY;
	dst.w *= scaleX * .5;
	dst.h *= scaleY * .5;

	dst.x += pivotX - positionX;
	dst.y += pivotY - positionY;
}

void Renderer::DrawRect(const std::shared_ptr<Texture> &texture, const SDL_FRect &dst, const SDL_FRect &src, std::float_t z) noexcept
{
	RenderCommand::DrawRect cmd{};

	cmd.tex = texture->GetSDLTextureHandle();
	cmd.dst = dst;
	cmd.src = src;
	cmd.ang = 0;
	cmd.nulls = RenderCommand::DrawRect::Ori;

	ApplyTransform(cmd.dst);

	_renderCommands.emplace_back(RenderCommand{
	    .type = RenderCommand::Type::DrawRect,
	    .cmd = {cmd},
	    .z = z,
	});
}

void Renderer::LuaDraw(const sol::table &args)
{
	RenderCommand::DrawRect cmd{};

	try
	{
		if (auto &&renderTarget = args.get_or("texture", static_cast<RenderTarget *>(nullptr)); renderTarget != nullptr)
		{
			cmd.tex = renderTarget->_texture->GetSDLTextureHandle();
		}
		else if (auto &&texture = GetOrCreateImageTexture(args.get_or<ImageID>("image", 0)); texture != nullptr)
		{
			cmd.tex = texture->GetSDLTextureHandle();
		}
		else [[unlikely]]
		{
			_log->Error("Failed to draw texture: require a RenderTarget 'texture' or valid ImageID 'image'");
			return;
		}

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
			cmd.nulls = RenderCommand::DrawRect::Nulls(cmd.nulls | RenderCommand::DrawRect::Src);
		}

		if (const sol::object &origin = args["origin"]; origin.is<sol::table>())
		{
			sol::table ori = origin.as<sol::table>();
			cmd.ori.x = ori.get_or(1, 0);
			cmd.ori.y = ori.get_or(2, 0);
		}
		else
		{
			cmd.nulls = RenderCommand::DrawRect::Nulls(cmd.nulls | RenderCommand::DrawRect::Ori);
		}

		cmd.ang = args.get_or("angle", 0.0f);
		std::float_t z = args.get_or("z", 0.0f);

		ApplyTransform(cmd.dst);

		auto &&renderCommands = _renderTargets.empty() ? _renderCommands : _renderTargets.top()->_renderCommands;

		renderCommands.emplace_back(RenderCommand{
		    RenderCommand::Type::DrawRect,
		    cmd,
		    SDL_Color(255, 255, 255, 255),
		    z,
		});
	}
	catch (std::exception &e)
	{
		GetScriptEngine().ThrowError("C++ exception in `Renderer::DrawImage`: {}", e.what());
	}
}

std::shared_ptr<RenderTarget> Renderer::LuaNewRenderTarget(const sol::object &width, const sol::object &height)
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

	if (auto &&texture = renderTarget->_texture; texture != nullptr)
	{
		SetRenderTexture(texture);
	}
}

void Renderer::LuaBeginTarget(const std::shared_ptr<RenderTarget> &renderTarget) noexcept
{
	try
	{
		BeginTarget(renderTarget);
	}
	catch (std::exception &e)
	{
		assert(false && "UNHANDLED ERROR");
	}
}

bool Renderer::EndTarget(const std::optional<SDL_FRect> &source, const std::optional<SDL_FRect> &destination) noexcept
{
	if (_renderTargets.empty())
	{
		return false;
	}

	std::shared_ptr<RenderTarget> &renderTarget = _renderTargets.top();
	_renderTargets.pop();

	SetRenderTexture(renderTarget->_texture);
	RenderImpl(renderTarget->_renderCommands);

	SetRenderTexture(_renderTargets.empty() ? nullptr : _renderTargets.top()->_texture);

	SDL_FRect src;
	if (source.has_value())
	{
		src = source.value();
	}
	else
	{
		auto [width, height] = renderTarget->_texture->GetSize();
		src = {0, 0, width, height};
	}

	SDL_FRect dst;
	if (destination.has_value())
	{
		dst = destination.value();
	}
	else
	{
		auto [width, height] = _window.GetSize();
		dst = {0, 0, std::float_t(width), std::float_t(height)};
	}

	SDL_RenderTexture(_sdlRenderer, renderTarget->_texture->GetSDLTextureHandle(), &src, &dst);

	return true;
}

bool Renderer::LuaEndTarget(const sol::object &source, const sol::object &destination) noexcept
{
	try
	{
		std::optional<SDL_FRect> srcRect;
		if (source.is<sol::table>())
		{
			auto &&src = source.as<sol::table>();
			auto x = src.get_or<std::float_t>(1, 0);
			auto y = src.get_or<std::float_t>(2, 0);
			auto w = src.get_or<std::float_t>(3, 0);
			auto h = src.get_or<std::float_t>(4, 0);
			srcRect = {x, y, w, h};
		}
		else
		{
			srcRect = std::nullopt;
		}

		std::optional<SDL_FRect> dstRect;
		if (destination.is<sol::table>())
		{
			auto &&dst = destination.as<sol::table>();
			auto x = dst.get_or<std::float_t>(1, 0);
			auto y = dst.get_or<std::float_t>(2, 0);
			auto w = dst.get_or<std::float_t>(3, 0);
			auto h = dst.get_or<std::float_t>(4, 0);
			dstRect = {x, y, w, h};
		}
		else
		{
			dstRect = std::nullopt;
		}

		return EndTarget(srcRect, dstRect);
	}
	catch (std::exception &e)
	{
		assert(false && "UNHANDLED ERROR");
		return false;
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

void Renderer::RenderImpl(std::vector<RenderCommand> &renderCommands) noexcept
{
	std::sort(renderCommands.begin(), renderCommands.end());

	for (auto &&command : renderCommands)
	{
		switch (command.type)
		{
		case RenderCommand::Type::DrawRect:
		{
			auto &args = command.cmd.drawRect;
			SDL_FRect *src = (args.nulls & RenderCommand::DrawRect::Src) ? nullptr : &args.src;
			SDL_FPoint *ori = (args.nulls & RenderCommand::DrawRect::Ori) ? nullptr : &args.ori;
			SDL_RenderTextureRotated(_sdlRenderer, args.tex, src, &args.dst, args.ang, ori, SDL_FlipMode::SDL_FLIP_NONE);
			break;
		}
		}
	}

	renderCommands.clear();
}

void Renderer::Render() noexcept
{
	if (_renderTargets.empty())
	{
		RenderImpl(_renderCommands);
	}
	else
	{
		RenderImpl(_renderTargets.top()->_renderCommands);
	}
}

void Renderer::Begin() noexcept
{
	SDL_SetRenderTarget(_sdlRenderer, nullptr);
	SDL_SetRenderDrawColor(_sdlRenderer, 0, 0, 0, 255);
	SDL_RenderClear(_sdlRenderer);
	SDL_SetRenderTarget(_sdlRenderer, _sdlTextureMain);
	SDL_RenderClear(_sdlRenderer);

	if (GetEngine().GetLoadingState() == Engine::ELoadingState::InProgress)
	{
		SDL_RenderTexture(_sdlRenderer, _sdlTextureBackground, nullptr, nullptr);
	}
}

void Renderer::End() noexcept
{
	SDL_SetRenderTarget(_sdlRenderer, nullptr);
	SDL_RenderTexture(_sdlRenderer, _sdlTextureMain, nullptr, nullptr);

	// Resize textures
	{
		auto [w1, h1] = _window.GetSize();
		std::float_t w2, h2;
		SDL_GetTextureSize(_sdlTextureMain, &w2, &h2);

		if (std::float_t(w1) != w2 || std::float_t(h1) != h2) [[unlikely]]
		{
			_sdlTextureMain = SDL_CreateTexture(_sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w1, h1);
			_sdlTextureBackground = SDL_CreateTexture(_sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w1, h1);
		}
	}

	if (GetEngine().GetLoadingState() != Engine::ELoadingState::InProgress)
	{
		SDL_SetRenderTarget(_sdlRenderer, _sdlTextureBackground);
		SDL_SetRenderDrawColor(_sdlRenderer, 0, 0, 0, 255);
		SDL_RenderClear(_sdlRenderer);
		SDL_RenderTexture(_sdlRenderer, _sdlTextureMain, nullptr, nullptr);
	}

	SDL_RenderPresent(_sdlRenderer);

	if (!_renderTargets.empty())
	{
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
