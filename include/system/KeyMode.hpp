/**
 * @file system/KeyMode.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "SDL3/SDL_keycode.h"

#include <cstdint>

namespace tudov
{
	enum class EKeyMode : std::uint32_t
	{
		None = SDL_KMOD_NONE,         // No modifier is applicable.
		LeftShift = SDL_KMOD_LSHIFT,  // The left Shift key is down.
		RightShift = SDL_KMOD_RSHIFT, // The right Shift key is down.
		Level5 = SDL_KMOD_LEVEL5,     // The Level 5 Shift key is down.
		LeftCtrl = SDL_KMOD_LCTRL,    // The left Ctrl (Control) key is down.
		RightCtrl = SDL_KMOD_RCTRL,   // The right Ctrl (Control) key is down.
		LeftAlt = SDL_KMOD_LALT,      // The left Alt key is down.
		RightAlt = SDL_KMOD_RALT,     // The right Alt key is down.
		LeftGui = SDL_KMOD_LGUI,      // The left GUI key (often the Windows key) is down.
		RightGui = SDL_KMOD_RGUI,     // The right GUI key (often the Windows key) is down.
		Num = SDL_KMOD_NUM,           // The Num Lock key (may be located on an extended keypad) is down.
		Caps = SDL_KMOD_CAPS,         // The Caps Lock key is down.
		Mode = SDL_KMOD_MODE,         // The !AltGr key is down.
		Scroll = SDL_KMOD_SCROLL,     // The Scroll Lock key is down.
		Ctrl = SDL_KMOD_CTRL,         // Any Ctrl key is down.
		Shift = SDL_KMOD_SHIFT,       // Any Shift key is down.
		Alt = SDL_KMOD_ALT,           // Any Alt key is down.
		GUI = SDL_KMOD_GUI,           // Any GUI key is down.
	};
} // namespace tudov
