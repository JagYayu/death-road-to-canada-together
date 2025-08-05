// #pragma once

// #include "SDL3/SDL_render.h"
// #include "program/Context.hpp"

// #include <cmath>
// #include <tuple>

// namespace tudov
// {
// 	class Renderer;

// 	class Camera2D : public IContextProvider
// 	{
// 		friend Renderer;

// 	  public:
// 		enum class EViewScaleMode
// 		{
// 			Disable = SDL_LOGICAL_PRESENTATION_DISABLED,
// 			Stretch = SDL_LOGICAL_PRESENTATION_STRETCH,
// 			Letterbox = SDL_LOGICAL_PRESENTATION_LETTERBOX,
// 			Overscan = SDL_LOGICAL_PRESENTATION_OVERSCAN,
// 			IntegerScale = SDL_LOGICAL_PRESENTATION_INTEGER_SCALE,
// 		};

// 	  private:
// 		Renderer &_renderer;
// 		EViewScaleMode _viewScaleMode;

// 		std::float_t _positionLerpFactor;
// 		std::float_t _scaleLerpFactor;
// 		std::float_t _viewLerpFactor;

// 		std::float_t _positionX;
// 		std::float_t _positionY;
// 		std::float_t _scaleX;
// 		std::float_t _scaleY;
// 		std::float_t _viewX;
// 		std::float_t _viewY;

// 		std::float_t _targetPositionX;
// 		std::float_t _targetPositionY;
// 		std::float_t _targetScaleX;
// 		std::float_t _targetScaleY;
// 		std::float_t _targetViewX;
// 		std::float_t _targetViewY;

// 	  public:
// 		explicit Camera2D(Renderer &renderer) noexcept;
// 		~Camera2D() noexcept = default;

// 		Context &GetContext() noexcept override;

// 		std::float_t GetPositionLerpFactor() noexcept;
// 		void SetPositionLerpFactor(std::float_t factor) noexcept;
// 		std::float_t GetScaleLerpFactor() noexcept;
// 		void SetScaleLerpFactor(std::float_t factor) noexcept;
// 		std::float_t GetViewLerpFactor() noexcept;
// 		void SetViewLerpFactor(std::float_t factor) noexcept;

// 		std::tuple<std::float_t, std::float_t> GetCameraTargetPosition() noexcept;
// 		void SetCameraTargetPosition(std::float_t x, std::float_t y) noexcept;
// 		std::tuple<std::float_t, std::float_t> GetCameraPosition() noexcept;
// 		std::tuple<std::float_t, std::float_t> GetCameraTargetScale() noexcept;
// 		void SetCameraTargetScale(std::float_t x, std::float_t y) noexcept;
// 		std::tuple<std::float_t, std::float_t> GetCameraScale() noexcept;

// 		void Reset() noexcept;
// 		void Update() noexcept;
// 	};
// } // namespace tudov
