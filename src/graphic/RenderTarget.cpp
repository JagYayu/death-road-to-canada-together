#include "graphic/RenderTarget.hpp"

#include "graphic/Renderer.hpp"
#include "program/Engine.hpp"
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
    : _renderer(renderer),
      _positionLerpFactor(0.95f),
      _scaleLerpFactor(0.975f),
      _viewLerpFactor(0.925f),
      _positionX(0.0f),
      _positionY(0.0f),
      _scaleX(1.0f),
      _scaleY(1.0f),
      _viewX(width),
      _viewY(height),
      _targetPositionX(0.0f),
      _targetPositionY(0.0f),
      _targetScaleX(1.0f),
      _targetScaleY(1.0f),
      _targetViewX(width),
      _targetViewY(height)
{
	_texture = std::make_shared<Texture>(_renderer);
	_texture->Initialize(width, height, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET);
}

RenderTarget::~RenderTarget() noexcept
{
}

Context &RenderTarget::GetContext() noexcept
{
	return _renderer.GetContext();
}

std::float_t RenderTarget::GetWidth() noexcept
{
	return _texture->GetWidth();
}

std::float_t RenderTarget::GetHeight() noexcept
{
	return _texture->GetHeight();
}

std::tuple<std::float_t, std::float_t> RenderTarget::GetSize() noexcept
{
	return _texture->GetSize();
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

std::float_t RenderTarget::GetPositionLerpFactor() noexcept
{
	return _positionLerpFactor;
}

void RenderTarget::SetPositionLerpFactor(std::float_t factor) noexcept
{
	_positionLerpFactor = factor;
}

std::float_t RenderTarget::GetScaleLerpFactor() noexcept
{
	return _scaleLerpFactor;
}

void RenderTarget::SetScaleLerpFactor(std::float_t factor) noexcept
{
	_scaleLerpFactor = factor;
}

std::float_t RenderTarget::GetViewLerpFactor() noexcept
{
	return _viewLerpFactor;
}

void RenderTarget::SetViewLerpFactor(std::float_t factor) noexcept
{
	_viewLerpFactor = factor;
}

std::tuple<std::float_t, std::float_t> RenderTarget::GetTargetPosition() noexcept
{
	return std::make_tuple(_targetPositionX, _targetPositionY);
}

void RenderTarget::SetTargetPosition(std::float_t x, std::float_t y) noexcept
{
	_targetPositionX = x;
	_targetPositionY = y;
}

std::tuple<std::float_t, std::float_t> RenderTarget::GetPosition() noexcept
{
	return std::make_tuple(_positionX, _positionY);
}

std::tuple<std::float_t, std::float_t> RenderTarget::GetTargetScale() noexcept
{
	return std::make_tuple(_targetScaleX, _targetScaleY);
}

void RenderTarget::SetTargetScale(std::float_t x, std::float_t y) noexcept
{
	_targetScaleX = x;
	_targetScaleY = y;
}

std::tuple<std::float_t, std::float_t> RenderTarget::GetScale() noexcept
{
	return std::make_tuple(_scaleX, _scaleY);
}

std::tuple<std::float_t, std::float_t> RenderTarget::GetTargetViewSize() noexcept
{
	return std::make_tuple(_targetViewX, _targetViewY);
}

void RenderTarget::SetTargetViewSize(std::float_t x, std::float_t y) noexcept
{
	_targetViewX = x;
	_targetViewY = y;
}

std::tuple<std::float_t, std::float_t> RenderTarget::GetViewSize() noexcept
{
	return std::make_tuple(_viewX, _viewY);
}

RenderTarget::EViewScaleMode RenderTarget::GetViewScaleMode() noexcept
{
	return _viewScaleMode;
}

void RenderTarget::SetViewScaleMode(EViewScaleMode mode) noexcept
{
	_viewScaleMode = mode;
}

void RenderTarget::Update() noexcept
{
	SDL_RendererLogicalPresentation mode;
	switch (_viewScaleMode)
	{
	case EViewScaleMode::Stretch:
		mode = SDL_LOGICAL_PRESENTATION_STRETCH;
	case EViewScaleMode::Letterbox:
		mode = SDL_LOGICAL_PRESENTATION_LETTERBOX;
	case EViewScaleMode::Overscan:
		mode = SDL_LOGICAL_PRESENTATION_OVERSCAN;
	case EViewScaleMode::IntegerScale:
		mode = SDL_LOGICAL_PRESENTATION_INTEGER_SCALE;
	default:
		mode = SDL_LOGICAL_PRESENTATION_DISABLED;
	}

	std::float_t deltaTime = GetEngine().GetDeltaTime();

	std::float_t tPosition = 1.0f - std::exp(-_positionLerpFactor * deltaTime);
	std::float_t tScale = 1.0f - std::exp(-_positionLerpFactor * deltaTime);
	std::float_t tView = 1.0f - std::exp(-_positionLerpFactor * deltaTime);

	_positionX = std::lerp(_positionX, _targetPositionX, tPosition);
	_positionY = std::lerp(_positionY, _targetPositionY, tPosition);
	_scaleX = std::lerp(_scaleX, _targetScaleX, tScale);
	_scaleY = std::lerp(_scaleY, _targetScaleY, tScale);
	_viewX = std::lerp(_viewX, _targetViewX, tView);
	_viewY = std::lerp(_viewY, _targetViewY, tView);

	auto sdlRenderer = _renderer.GetSDLRendererHandle();
	SDL_SetRenderScale(sdlRenderer, _scaleX, _scaleY);
	SDL_SetRenderLogicalPresentation(sdlRenderer, _viewX, _viewY, mode);
}
