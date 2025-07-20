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
	_texture = std::make_shared<Texture>(_renderer);
	_texture->Initialize(width, height, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET);
}

RenderTarget::~RenderTarget() noexcept
{
}

SDL_FRect RenderTarget::GetSource() noexcept
{
	if (_source.has_value())
	{
		return _source.value();
	}

	auto [width, height] = _texture->GetSize();
	return SDL_FRect(0, 0, width, height);
}

void RenderTarget::SetSource(const std::optional<SDL_FRect> &value) noexcept
{
	_source = value;
}

SDL_FRect RenderTarget::GetDestination() noexcept
{
	if (_destination.has_value())
	{
		return _destination.value();
	}

	auto [width, height] = _renderer.GetWindow().GetSize();
	return SDL_FRect(0, 0, width, height);
}

void RenderTarget::SetDestination(const std::optional<SDL_FRect> &value) noexcept
{
	_destination = value;
}

TE_FORCEINLINE void AssertTextureExpired(const std::weak_ptr<Texture> &texture)
{
	if (texture.expired()) [[unlikely]]
	{
		throw std::runtime_error("Texture was already been destroyed from renderer");
	}
}

bool RenderTarget::Resize(std::int32_t width, std::int32_t height)
{
	// AssertTextureExpired(_texture);

	{
		auto [prevWidth, prevHeight] = _texture->GetSize();
		if (prevWidth == width && prevHeight == height)
		{
			return false;
		}
	}

	_texture = std::make_shared<Texture>(_renderer);
	_texture->Initialize(width, height, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET);
	return true;
}

bool RenderTarget::ResizeToFit()
{
	auto &&[width, height] = _renderer.GetWindow().GetSize();
	return Resize(width, height);
}

void RenderTarget::Draw(const SDL_FRect &dst, std::float_t z)
{
	// AssertTextureExpired(_texture);

	auto [width, height] = _texture->GetSize();
	_renderer.DrawTexture(_texture, dst, SDL_FRect{0, 0, width, height}, z);
}

void RenderTarget::Draw(const SDL_FRect &dst, const SDL_FRect &src, std::float_t z)
{
	// AssertTextureExpired(_texture);

	_renderer.DrawTexture(_texture, dst, src, z);
}
