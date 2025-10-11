/**
 * @file program/Keyboard.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "System/Keyboard.hpp"

#include "Event/AppEvent.hpp"
#include "Event/CoreEvents.hpp"
#include "Event/CoreEventsData.hpp"
#include "Event/EventManager.hpp"
#include "Event/RuntimeEvent.hpp"
#include "Mod/ScriptEngine.hpp"
#include "Program/WindowManager.hpp"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_keycode.h"
#include "System/Keyboard.hpp"
#include "System/LogMicros.hpp"
#include "System/ScanCode.hpp"
#include "Util/Micros.hpp"
#include "Util/Utils.hpp"

#include "SDL3/SDL_events.h"

#include <memory>

using namespace tudov;

Keyboard::Keyboard(Context &context, KeyboardID keyboardID) noexcept
    : _context(context),
      _keyboardID(keyboardID)
{
}

Context &Keyboard::GetContext() noexcept
{
	return _context;
}

Log &Keyboard::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(Keyboard));
}

KeyboardID Keyboard::GetKeyboardID() const noexcept
{
	return _keyboardID;
}

bool Keyboard::HandleEvent(AppEvent &appEvent) noexcept
{
	auto &sdlEvent = *appEvent.sdlEvent;

	switch (sdlEvent.type)
	{
	case SDL_EVENT_KEY_DOWN:
	case SDL_EVENT_KEY_UP:
	{
		SDL_KeyboardEvent &key = sdlEvent.key;
		KeyboardID keyboardID = key.which;
		if (keyboardID != _keyboardID)
		{
			return false;
		}

		EScanCode scanCode = static_cast<EScanCode>(key.scancode);
		EKeyCode keyCode = static_cast<EKeyCode>(key.key);
		std::shared_ptr<IWindow> window = GetWindowManager().GetIWindowByID(key.windowID);
		WindowID windowID = window != nullptr ? window->GetWindowID() : 0;

		RuntimeEvent *runtimeEvent;
		if (key.down)
		{
			runtimeEvent = &GetEventManager().GetCoreEvents().KeyboardPress();

			OnKeyDown(windowID, keyCode);
		}
		else if (!key.repeat)
		{
			runtimeEvent = &GetEventManager().GetCoreEvents().KeyboardRelease();

			OnKeyUp(windowID, keyCode);
		}
		else
		{
			runtimeEvent = &GetEventManager().GetCoreEvents().KeyboardRepeat();
		}

		EventKeyboardData data{
		    .window = std::dynamic_pointer_cast<Window>(window),
		    .windowID = windowID,
		    .keyboard = keyboardID != 0 ? shared_from_this() : nullptr,
		    .keyboardID = keyboardID,
		    .scanCode = scanCode,
		    .keyCode = keyCode,
		    .modifier = static_cast<EKeyModifier>(key.mod),
		};

		runtimeEvent->Invoke(&data, EventHandleKey(_keyboardID));

		break;
	}
	case SDL_EVENT_TEXT_EDITING:
	{
		break;
	}
	case SDL_EVENT_TEXT_INPUT:
	{
		break;
	}
	case SDL_EVENT_KEYMAP_CHANGED:
	{
		break;
	}
	case SDL_EVENT_KEYBOARD_ADDED:
	{
		SDL_KeyboardEvent &key = sdlEvent.key;

		//

		break;
	}
	case SDL_EVENT_KEYBOARD_REMOVED:
	{
		break;
	}
	case SDL_EVENT_TEXT_EDITING_CANDIDATES:
	{
		break;
	}
	default:
		return false;
	}

	TE_TRACE("Event handled by keyboard {}", _keyboardID);

	return true;
}

void Keyboard::OnKeyUp(WindowID windowID, EKeyCode keyCode) noexcept
{
	if (auto it = _windowHoldingKeyCodes.find(windowID); it != _windowHoldingKeyCodes.end()) [[likely]]
	{
		auto &entry = it->second;
		if (!entry.set.contains(keyCode)) [[unlikely]]
		{
			TE_WARN("Key code '{}' was already up", Utils::ToUnderlying(keyCode));
		}
		else
		{
			std::erase(entry.list, keyCode);
			entry.set.erase(keyCode);

			if (entry.list.empty() && entry.set.empty())
			{
				_windowHoldingKeyCodes.erase(windowID);
			}
		}
	}
}

void Keyboard::OnKeyDown(WindowID windowID, EKeyCode keyCode) noexcept
{
	if (auto &entry = _windowHoldingKeyCodes[windowID]; !entry.set.contains(keyCode)) [[likely]]
	{
		if (entry.set.contains(keyCode)) [[unlikely]]
		{
			TE_WARN("Key code '{}' was already down", Utils::ToUnderlying(keyCode));
		}
		else
		{
			entry.list.emplace_back(keyCode);
			entry.set.emplace(keyCode);
		}
	}
}

bool Keyboard::IsKeyCodeHeld(EKeyCode keyCode, WindowID windowID) noexcept
{
	auto it = _windowHoldingKeyCodes.find(windowID);
	if (it == _windowHoldingKeyCodes.end())
	{
		return false;
	}
	return it->second.set.contains(keyCode);
}

bool Keyboard::IsScanCodeHeld(EScanCode scanCode, WindowID windowID) noexcept
{
	auto it = _windowHoldingScanCodes.find(windowID);
	if (it == _windowHoldingScanCodes.end())
	{
		return false;
	}
	return it->second.set.contains(scanCode);
}

const std::vector<EKeyCode> *Keyboard::GetHeldKeyCodes(WindowID windowID) const noexcept
{
	auto it = _windowHoldingKeyCodes.find(windowID);
	if (it == _windowHoldingKeyCodes.end())
	{
		return nullptr;
	}
	return &it->second.list;
}

const std::vector<EScanCode> *Keyboard::GetHeldScanCodes(WindowID windowID) const noexcept
{
	auto it = _windowHoldingScanCodes.find(windowID);
	if (it == _windowHoldingScanCodes.end())
	{
		return nullptr;
	}
	return &it->second.list;
}

TE_FORCEINLINE WindowID LuaObjectToWindowID(sol::object windowID) noexcept
{
	return windowID.is<WindowID>() ? windowID.as<WindowID>() : 0;
}

bool Keyboard::LuaIsKeyCodeHeld(sol::object keyCode, sol::object windowID) noexcept
{
	if (!keyCode.is<EKeyCode>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("Bad argument to #1 '{}': number expected", TE_NAMEOF(keyCode));
	}

	return IsKeyCodeHeld(keyCode.as<EKeyCode>(), LuaObjectToWindowID(windowID));
}

bool Keyboard::LuaIsScanCodeHeld(sol::object scanCode, sol::object windowID) noexcept
{
	if (!scanCode.is<EScanCode>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("Bad argument to #1 '{}': number expected", TE_NAMEOF(scanCode));
	}

	return IsKeyCodeHeld(scanCode.as<EKeyCode>(), LuaObjectToWindowID(windowID));
}

template <typename T>
sol::table LuaListHeldCodesImpl(Keyboard *this_, sol::object windowID, std::unordered_map<WindowID, impl::KeyboardCodeEntry<T>> windowHoldingCodes) noexcept
{
	auto it = windowHoldingCodes.find(LuaObjectToWindowID(windowID));
	if (it == windowHoldingCodes.end())
	{
		return this_->GetScriptEngine().CreateTable();
	}

	auto &list = it->second.list;
	sol::table result = this_->GetScriptEngine().CreateTable(list.size(), 0);
	for (std::size_t index = 0; index < list.size(); ++index)
	{
		result[index + 1] = list[index];
	}

	return result;
}

sol::table Keyboard::LuaListHeldKeyCodes(sol::object windowID) noexcept
{
	return LuaListHeldCodesImpl(this, windowID, _windowHoldingKeyCodes);
}

sol::table Keyboard::LuaListHeldScanCodes(sol::object windowID) noexcept
{
	return LuaListHeldCodesImpl(this, windowID, _windowHoldingScanCodes);
}

PrimaryKeyboard::PrimaryKeyboard(Context &context) noexcept
    : Keyboard(context, 0)
{
}

Log &PrimaryKeyboard::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(PrimaryKeyboard));
}

bool PrimaryKeyboard::IsKeyCodeHeld(EKeyCode keyCode, WindowID windowID) noexcept
{
	return IsScanCodeHeld(static_cast<EScanCode>(SDL_GetScancodeFromKey(static_cast<SDL_Keycode>(keyCode), nullptr)), windowID);
}

bool PrimaryKeyboard::IsScanCodeHeld(EScanCode scanCode, WindowID windowID) noexcept
{
	std::int32_t count;
	const bool *state = SDL_GetKeyboardState(&count);
	auto index = static_cast<std::size_t>(scanCode);

	if (index >= count || index < 0)
	{
		return state[index];
	}
	else
	{
		return false;
	}
}
