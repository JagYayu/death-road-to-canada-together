/**
 * @file graphic/Camera2D.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

// #include "Graphic/Camera2D.hpp"

// #include "Graphic/Renderer.hpp"
// #include "Program/Engine.hpp"

// #include "SDL3/SDL_render.h"
// #include <cmath>

// using namespace tudov;

// Camera2D::Camera2D(Renderer &renderer) noexcept
//     : _renderer(renderer),
//       _viewScaleMode(EViewScaleMode::Letterbox),
//       _positionLerpFactor(0.75f),
//       _scaleLerpFactor(0.8f),
//       _viewLerpFactor(0.85f),
//       _positionX(0.0f),
//       _positionY(0.0f),
//       _scaleX(1.0f),
//       _scaleY(1.0f),
//       _viewX(1920.0f),
//       _viewY(1080.0f),
//       _targetPositionX(0.0f),
//       _targetPositionY(0.0f),
//       _targetScaleX(1.0f),
//       _targetScaleY(1.0f),
//       _targetViewX(1920.0f),
//       _targetViewY(1080.0f)
// {
// }

// Context &Camera2D::GetContext() noexcept
// {
// 	return _renderer.GetContext();
// }

// std::float_t Camera2D::GetPositionLerpFactor() noexcept
// {
// 	return _positionLerpFactor;
// }

// void Camera2D::SetPositionLerpFactor(std::float_t factor) noexcept
// {
// 	_positionLerpFactor = factor;
// }

// std::float_t Camera2D::GetScaleLerpFactor() noexcept
// {
// 	return _scaleLerpFactor;
// }

// void Camera2D::SetScaleLerpFactor(std::float_t factor) noexcept
// {
// 	_scaleLerpFactor = factor;
// }

// std::float_t Camera2D::GetViewLerpFactor() noexcept
// {
// 	return _viewLerpFactor;
// }

// void Camera2D::SetViewLerpFactor(std::float_t factor) noexcept
// {
// 	_viewLerpFactor = factor;
// }

// std::tuple<std::float_t, std::float_t> Camera2D::GetCameraTargetPosition() noexcept
// {
// 	return std::make_tuple(_targetPositionX, _targetPositionY);
// }

// void Camera2D::SetCameraTargetPosition(std::float_t x, std::float_t y) noexcept
// {
// 	_targetPositionX = x;
// 	_targetPositionY = y;
// }

// std::tuple<std::float_t, std::float_t> Camera2D::GetCameraPosition() noexcept
// {
// 	return std::make_tuple(_positionX, _positionY);
// }

// std::tuple<std::float_t, std::float_t> Camera2D::GetCameraTargetScale() noexcept
// {
// 	return std::make_tuple(_targetScaleX, _targetScaleY);
// }

// void Camera2D::SetCameraTargetScale(std::float_t x, std::float_t y) noexcept
// {
// 	_targetScaleX = x;
// 	_targetScaleY = y;
// }

// std::tuple<std::float_t, std::float_t> Camera2D::GetCameraScale() noexcept
// {
// 	return std::make_tuple(_scaleX, _scaleY);
// }

// void Camera2D::Update() noexcept
// {
// 	// SDL_RendererLogicalPresentation mode;
// 	// switch (_viewScaleMode)
// 	// {
// 	// case EViewScaleMode::Stretch:
// 	// 	mode = SDL_LOGICAL_PRESENTATION_STRETCH;
// 	// case EViewScaleMode::Letterbox:
// 	// 	mode = SDL_LOGICAL_PRESENTATION_LETTERBOX;
// 	// case EViewScaleMode::Overscan:
// 	// 	mode = SDL_LOGICAL_PRESENTATION_OVERSCAN;
// 	// case EViewScaleMode::IntegerScale:
// 	// 	mode = SDL_LOGICAL_PRESENTATION_INTEGER_SCALE;
// 	// default:
// 	// 	mode = SDL_LOGICAL_PRESENTATION_DISABLED;
// 	// }

// 	std::float_t deltaTime = GetEngine().GetDeltaTime();

// 	std::float_t tPosition = 1.0f - std::exp(-_positionLerpFactor * deltaTime);
// 	std::float_t tScale = 1.0f - std::exp(-_positionLerpFactor * deltaTime);

// 	_positionX = std::lerp(_positionX, _targetPositionX, tPosition);
// 	_positionY = std::lerp(_positionY, _targetPositionY, tPosition);
// 	_scaleX = std::lerp(_scaleX, _targetScaleX, tScale);
// 	_scaleY = std::lerp(_scaleY, _targetScaleY, tScale);

// 	auto sdlRenderer = _renderer.GetSDLRendererHandle();
// 	SDL_SetRenderScale(sdlRenderer, _scaleX, _scaleY);
// }
