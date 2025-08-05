#pragma once

#include "program/Context.hpp"

#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"

#include <cmath>
#include <memory>
#include <optional>
#include <tuple>

namespace tudov
{
	class Engine;
	class LuaAPI;
	class Renderer;
	class Texture;

	class RenderTarget : public std::enable_shared_from_this<RenderTarget>, public IContextProvider
	{
		friend LuaAPI;
		friend Renderer;

	  public:
		enum class EViewScaleMode
		{
			Disable = SDL_LOGICAL_PRESENTATION_DISABLED,
			Stretch = SDL_LOGICAL_PRESENTATION_STRETCH,
			Letterbox = SDL_LOGICAL_PRESENTATION_LETTERBOX,
			Overscan = SDL_LOGICAL_PRESENTATION_OVERSCAN,
			IntegerScale = SDL_LOGICAL_PRESENTATION_INTEGER_SCALE,
		};

	  private:
		Renderer &_renderer;
		std::shared_ptr<Texture> _texture;
		std::optional<SDL_FRect> _source;
		std::optional<SDL_FRect> _destination;

		EViewScaleMode _viewScaleMode;
		std::float_t _positionLerpFactor;
		std::float_t _scaleLerpFactor;
		std::float_t _viewLerpFactor;

		std::float_t _cameraX;
		std::float_t _cameraY;
		std::float_t _cameraScaleX;
		std::float_t _cameraScaleY;
		bool _snapCameraPosition;
		bool _snapCameraScale;
		std::float_t _viewX;
		std::float_t _viewY;

		std::float_t _targetPositionX;
		std::float_t _targetPositionY;
		std::float_t _targetScaleX;
		std::float_t _targetScaleY;
		std::float_t _targetViewX;
		std::float_t _targetViewY;

	  public:
		explicit RenderTarget(Renderer &renderer, std::int32_t width, std::int32_t height) noexcept;
		~RenderTarget() noexcept;

	  public:
		Context &GetContext() noexcept override;

		std::float_t GetWidth() noexcept;
		std::float_t GetHeight() noexcept;
		std::tuple<std::float_t, std::float_t> GetSize() noexcept;

		std::shared_ptr<Texture> GetTexture() const noexcept;

		SDL_FRect GetSource() noexcept;
		void SetSource(const std::optional<SDL_FRect> &value) noexcept;
		SDL_FRect GetDestination() noexcept;
		void SetDestination(const std::optional<SDL_FRect> &value) noexcept;
		bool Resize(std::int32_t width, std::int32_t height);
		bool ResizeToFit();

		std::float_t GetPositionLerpFactor() noexcept;
		void SetPositionLerpFactor(std::float_t factor) noexcept;
		std::float_t GetScaleLerpFactor() noexcept;
		void SetScaleLerpFactor(std::float_t factor) noexcept;

		std::tuple<std::float_t, std::float_t> GetCameraTargetPosition() noexcept;
		void SetCameraTargetPosition(std::float_t x, std::float_t y) noexcept;
		std::tuple<std::float_t, std::float_t> GetCameraPosition() noexcept;
		std::tuple<std::float_t, std::float_t> GetCameraTargetScale() noexcept;
		void SetCameraTargetScale(std::float_t x, std::float_t y) noexcept;
		std::tuple<std::float_t, std::float_t> GetCameraScale() noexcept;

		void SnapCameraPosition() noexcept;
		void SnapCameraScale() noexcept;
		void Reset() noexcept;
		void Update() noexcept;
	};
} // namespace tudov
