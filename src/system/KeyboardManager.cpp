/**
 * @file program/KeyboardManager.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "system/KeyboardManager.hpp"

#include "event/AppEvent.hpp"
#include "event/CoreEventsData.hpp"
#include "event/EventManager.hpp"
#include "program/WindowManager.hpp"
#include "system/Keyboard.hpp"
#include "util/Definitions.hpp"
#include "util/Micros.hpp"

#include "SDL3/SDL_keyboard.h"

#include <memory>

using namespace tudov;

KeyboardManager::KeyboardManager(Context &context) noexcept
    : _context(context)
{
	std::int32_t count;
	SDL_KeyboardID *keyboardIDs = SDL_GetKeyboards(&count);

	_keyboardList.reserve(count);
	_keyboardMap.reserve(count);

	for (std::int32_t index = count; index < count; ++index)
	{
		auto id = keyboardIDs[index];
		auto keyboard = std::make_shared<Keyboard>(_context, id);

		_keyboardList.emplace_back(keyboard);
		_keyboardMap[id] = keyboard;
	}
}

Context &KeyboardManager::GetContext() noexcept
{
	return _context;
}

Log &KeyboardManager::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(KeyboardManager));
}

bool KeyboardManager::HandleEvent(AppEvent &appEvent) noexcept
{
	for (std::shared_ptr<IKeyboard> &keyboard : _keyboardList)
	{
		if (keyboard->HandleEvent(appEvent))
		{
			return true;
		}
	}
	return false;
}

std::shared_ptr<IKeyboard> KeyboardManager::GetKeyboardAt(std::int32_t index) noexcept
{
	if (index >= 0 && index < _keyboardList.size())
	{
		return _keyboardList.at(index);
	}
	return nullptr;
}

std::shared_ptr<IKeyboard> KeyboardManager::GetKeyboardByID(KeyboardID id) noexcept
{
	auto it = _keyboardMap.find(id);
	if (it != _keyboardMap.end())
	{
		return it->second;
	}
	return nullptr;
}

std::shared_ptr<Keyboard> KeyboardManager::LuaGetKeyboardAt(sol::object index) noexcept
{
	return std::dynamic_pointer_cast<Keyboard>(GetKeyboardAt(index.is<std::int32_t>() ? index.as<std::int32_t>() : 0));
}

std::shared_ptr<Keyboard> KeyboardManager::LuaGetKeyboardByID(sol::object id) noexcept
{
	return std::dynamic_pointer_cast<Keyboard>(GetKeyboardByID(id.is<KeyboardID>() ? id.as<KeyboardID>() : 0));
}
