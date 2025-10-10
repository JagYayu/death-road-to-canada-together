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

#include "Event/AppEvent.hpp"

#include "SDL3/SDL_events.h"

#include <memory>

using namespace tudov;

AppEvent::AppEvent(const SDL_Event &sdlEvent) noexcept
    : sdlEvent(std::make_unique<SDL_Event>(sdlEvent))
{
}
