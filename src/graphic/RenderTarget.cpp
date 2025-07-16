#include "RenderTarget.hpp"

#include "Renderer.hpp"
#include "program/Window.hpp"
#include "util/Micros.hpp"
#include "util/MicrosImpl.hpp"

#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"

#include <stdexcept>

using namespace tudov;

#define TUDOV_GEN_LUA_P_CALL(Function, Block)                                              \
	try                                                                                    \
	{                                                                                      \
		Block;                                                                             \
	}                                                                                      \
	catch (std::exception & e)                                                             \
	{                                                                                      \
		Log::Get("RenderTarget")->Error("C++ exception in `" #Function "`: {}", e.what()); \
	}                                                                                      \
	TE_GEN_END

RenderTarget::RenderTarget(Renderer &renderer, std::int32_t width, std::int32_t height) noexcept
    : _renderer(renderer)
{
	_texture = _renderer.CreateTexture(width, height, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET);
}

RenderTarget::~RenderTarget() noexcept
{
	if (!_texture.expired())
	{
		_renderer.DestroyTexture(_texture.lock());
	}
}

SDL_Rect &RenderTarget::GetRect() noexcept
{
	return _rect;
}

TE_FORCEINLINE void AssertTextureExpired(std::weak_ptr<Texture> &texture)
{
	if (texture.expired()) [[unlikely]]
	{
		throw std::runtime_error("Texture was already been destroyed from renderer");
	}
}

void RenderTarget::BeginTarget()
{
	AssertTextureExpired(_texture);

	_renderer.SetRenderTexture(_texture.lock());
}

void RenderTarget::LuaBeginTarget() noexcept
{
	try
	{
		BeginTarget();
	}
	catch (std::exception &e)
	{
		Log::Get("RenderTarget")->Error("C++ exception in `RenderTarget::BeginTarget`: {}", e.what());
	}
}

void RenderTarget::EndTarget()
{
	AssertTextureExpired(_texture);

	if (_renderer.GetRenderTexture().get() != _texture.lock().get()) [[unlikely]]
	{
		throw std::runtime_error("Current render target was not bounded to renderer");
	}

	_renderer.Render();
	_renderer.SetRenderTexture(nullptr);
}

void RenderTarget::LuaEndTarget() noexcept
{
	try
	{
		EndTarget();
	}
	catch (std::exception &e)
	{
		Log::Get("RenderTarget")->Error("C++ exception in `RenderTarget::EndTarget`: {}", e.what());
	}
}

bool RenderTarget::Resize(std::int32_t width, std::int32_t height)
{
	AssertTextureExpired(_texture);

	{
		auto &&texture = _texture.lock();
		auto [prevWidth, prevHeight] = texture->GetSize();
		if (prevWidth == width && prevHeight == height)
		{
			return false;
		}

		_renderer.DestroyTexture(texture);
	}

	_texture = _renderer.CreateTexture(width, height, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET);
	return true;
}

bool RenderTarget::ResizeToFit()
{
	auto &&[width, height] = _renderer.GetWindow().GetSize();
	return Resize(width, height);
}

void RenderTarget::Draw(const SDL_FRect &dst, std::float_t z)
{
	AssertTextureExpired(_texture);

	auto &&texture = _texture.lock();
	auto [width, height] = texture->GetSize();
	_renderer.DrawTexture(texture, dst, SDL_FRect{0, 0, width, height}, z);
}

void RenderTarget::Draw(const SDL_FRect &dst, const SDL_FRect &src, std::float_t z)
{
	AssertTextureExpired(_texture);

	_renderer.DrawTexture(_texture.lock(), dst, src, z);
}
