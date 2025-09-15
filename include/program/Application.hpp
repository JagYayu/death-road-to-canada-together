/**
 * @file program/Application.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "util/Version.hpp"

union SDL_Event;

namespace tudov
{
	struct AppEvent;

	/**
	 * @brief Tudov engine base class
	 */
	struct Application
	{
		virtual ~Application() noexcept = default;

		virtual Version GetAppVersion() const noexcept = 0;

		virtual void Initialize() noexcept = 0;

		virtual bool Tick() noexcept = 0;

		virtual void Event(AppEvent &appEvent) noexcept = 0;

		virtual void Deinitialize() noexcept = 0;
	};
} // namespace tudov
