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

union SDL_Event;

namespace tudov
{
	/**
	 * @brief Tudov engine base class
	 */
	struct Application
	{
		virtual ~Application() noexcept = default;

		virtual void Initialize() noexcept = 0;
		virtual bool Tick() noexcept = 0;
		virtual void Event(SDL_Event &event) noexcept = 0;
		virtual void Deinitialize() noexcept = 0;
	};
} // namespace tudov
