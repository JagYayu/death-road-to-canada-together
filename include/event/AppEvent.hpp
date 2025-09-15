/**
 * @file event/AppEvent.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <memory>

union SDL_Event;

namespace tudov
{
	struct AppEvent
	{
		std::unique_ptr<SDL_Event> sdlEvent;

		explicit AppEvent(const SDL_Event &sdlEvent) noexcept;
	};
} // namespace tudov
