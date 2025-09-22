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

#include "system/Keyboard.hpp"

#include "event/AppEvent.hpp"
#include "event/CoreEvents.hpp"
#include "event/CoreEventsData.hpp"
#include "event/EventManager.hpp"
#include "event/RuntimeEvent.hpp"
#include "mod/ScriptEngine.hpp"
#include "program/WindowManager.hpp"
#include "system/Keyboard.hpp"
#include "system/LogMicros.hpp"
#include "system/ScanCode.hpp"
#include "util/Micros.hpp"
#include "util/Utils.hpp"

#include "SDL3/SDL_events.h"

#include <algorithm>

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

		auto scanCode = static_cast<EScanCode>(key.scancode);
		auto keyCode = static_cast<EKeyCode>(key.key);
		std::shared_ptr<IWindow> window = GetWindowManager().GetWindowByID(key.windowID);
		WindowID windowID = window != nullptr ? window->GetWindowID() : 0;

		RuntimeEvent *runtimeEvent;
		if (!key.down)
		{
			runtimeEvent = &GetEventManager().GetCoreEvents().KeyboardRelease();

			if (auto &entry = _windowHoldingKeyCodes[windowID]; !entry.set.contains(keyCode)) [[likely]]
			{
				entry.list.emplace_back(keyCode);
				entry.set.emplace(keyCode);
			}
			else [[unlikely]]
			{
				TE_WARN("Key code '{}' was already down", Utils::ToUnderlying(keyCode));
			}
		}
		else if (key.repeat)
		{
			runtimeEvent = &GetEventManager().GetCoreEvents().KeyboardRepeat();
		}
		else
		{
			runtimeEvent = &GetEventManager().GetCoreEvents().KeyboardPress();

			if (auto it = _windowHoldingKeyCodes.find(windowID); it != _windowHoldingKeyCodes.end()) [[likely]]
			{
				auto &entry = it->second;
				entry.list.erase(std::find(entry.list.begin(), entry.list.end(), keyCode));
				entry.set.erase(keyCode);

				if (entry.list.empty() && entry.set.empty())
				{
					_windowHoldingKeyCodes.erase(windowID);
				}
			}
			else [[unlikely]]
			{
				TE_WARN("Key code '{}' was already up", Utils::ToUnderlying(keyCode));
			}
		}

		EventKeyboardData data{
		    .window = window,
		    .windowID = windowID,
		    .keyboard = shared_from_this(),
		    .keyboardID = _keyboardID,
		    .scanCode = scanCode,
		    .keyCode = keyCode,
		    .modifier = static_cast<EKeyModifier>(key.mod),
		};

		runtimeEvent->Invoke(&data, _keyboardID, EEventInvocation::None);

		return true;
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
		break;
	}

	return false;
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
