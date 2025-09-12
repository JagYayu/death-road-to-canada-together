/**
 * @file graphic/RenderTarget.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

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
      _cameraX(0.0f),
      _cameraY(0.0f),
      _cameraScaleX(1.0f),
      _cameraScaleY(1.0f),
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

std::shared_ptr<Texture> RenderTarget::GetTexture() const noexcept
{
	return _texture;
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

	_targetViewX = width;
	_targetViewY = height;

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

std::tuple<std::float_t, std::float_t> RenderTarget::GetCameraTargetPosition() noexcept
{
	return std::make_tuple(_targetPositionX, _targetPositionY);
}

void RenderTarget::SetCameraTargetPosition(std::float_t x, std::float_t y) noexcept
{
	_targetPositionX = x;
	_targetPositionY = y;
}

std::tuple<std::float_t, std::float_t> RenderTarget::GetCameraPosition() noexcept
{
	return std::make_tuple(_cameraX, _cameraY);
}

std::tuple<std::float_t, std::float_t> RenderTarget::GetCameraTargetScale() noexcept
{
	return std::make_tuple(_targetScaleX, _targetScaleY);
}

void RenderTarget::SetCameraTargetScale(std::float_t x, std::float_t y) noexcept
{
	_targetScaleX = x;
	_targetScaleY = y;
}

std::tuple<std::float_t, std::float_t> RenderTarget::GetCameraScale() noexcept
{
	return std::make_tuple(_cameraScaleX, _cameraScaleY);
}

EBlendMode RenderTarget::GetBlendMode() noexcept
{
	return _renderer.GetBlendMode(_texture.get());
}

void RenderTarget::SetBlendMode(EBlendMode blendMode) noexcept
{
	_renderer.SetBlendMode(_texture.get(), blendMode);
}

void RenderTarget::SnapCameraPosition() noexcept
{
	_snapCameraPosition = true;
}

void RenderTarget::SnapCameraScale() noexcept
{
	_snapCameraScale = true;
}

void RenderTarget::Reset() noexcept
{
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

	if (_snapCameraPosition)
	{
		_cameraX = _targetPositionX;
		_cameraY = _targetPositionY;
		_snapCameraPosition = false;
	}
	else
	{
		_cameraX = std::lerp(_cameraX, _targetPositionX, tPosition);
		_cameraY = std::lerp(_cameraY, _targetPositionY, tPosition);
	}
	if (_snapCameraScale)
	{
		_cameraScaleX = _targetScaleX;
		_cameraScaleY = _targetScaleY;
		_snapCameraScale = false;
	}
	else
	{
		_cameraScaleX = std::lerp(_cameraScaleX, _targetScaleX, tScale);
		_cameraScaleY = std::lerp(_cameraScaleY, _targetScaleY, tScale);
	}

	// _viewX = std::lerp(_viewX, _targetViewX, tView);
	// _viewY = std::lerp(_viewY, _targetViewY, tView);
}
