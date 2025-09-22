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
#include "event/CoreEvents.hpp"
#include "event/CoreEventsData.hpp"
#include "event/EventManager.hpp"
#include "event/RuntimeEvent.hpp"
#include "mod/ScriptEngine.hpp"
#include "program/WindowManager.hpp"
#include "system/Keyboard.hpp"
#include "system/LogMicros.hpp"
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
	++count;

	_keyboardList.reserve(count);
	_keyboardMap.reserve(count);

	for (std::int32_t index = count; index < count; ++index)
	{
		auto id = keyboardIDs[index];
		AddKeyboard(std::make_shared<Keyboard>(_context, id));
	}
}

void KeyboardManager::AddKeyboard(const std::shared_ptr<Keyboard> &keyboard) noexcept
{
	_keyboardList.emplace_back(keyboard);
	_keyboardMap[keyboard->GetKeyboardID()] = keyboard;
	_keyboards.emplace_back(keyboard);
}

Context &KeyboardManager::GetContext() noexcept
{
	return _context;
}

Log &KeyboardManager::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(KeyboardManager));
}

void KeyboardManager::PreInitialize() noexcept
{
	EventHandleFunction func{[this](sol::object e, const EventHandleKey &key)
	{
		OnKeyboardDeviceAdded(e, key);
	}};

	try
	{
		GetEventManager().GetCoreEvents().KeyboardAdded().Add(func, TE_NAMEOF(KeyboardManager::OnKeyboardDeviceAdded), RuntimeEvent::DefaultOrders[0]);
	}
	catch (const std::exception &e)
	{
		TE_ERROR("Error adding event handler: {}", e.what());
	}
}

void KeyboardManager::PostDeinitialize() noexcept
{
	try
	{
		GetEventManager().GetCoreEvents().KeyboardAdded().Remove(TE_NAMEOF(KeyboardManager::OnKeyboardDeviceAdded));
	}
	catch (const std::exception &e)
	{
		TE_ERROR("Error removing event handler: {}", e.what());
	}
}

void KeyboardManager::OnKeyboardDeviceAdded(sol::object e, const EventHandleKey &key) noexcept
{
	auto data = CoreEventData::Extract<EventKeyboardDeviceData>(e);
	if (data == nullptr) [[unlikely]]
	{
		TE_WARN("Failed to add keyboard device!");

		return;
	}

	AddKeyboard(std::make_shared<Keyboard>(_context, data->keyboardID));
}

bool KeyboardManager::HandleEvent(AppEvent &appEvent) noexcept
{
	for (std::shared_ptr<Keyboard> &keyboard : _keyboardList)
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

std::vector<std::shared_ptr<IKeyboard>> *KeyboardManager::GetKeyboards() noexcept
{
	return &_keyboards;
}

std::shared_ptr<Keyboard> KeyboardManager::LuaGetKeyboardAt(sol::object index) noexcept
{
	return std::dynamic_pointer_cast<Keyboard>(GetKeyboardAt(index.is<std::int32_t>() ? index.as<std::int32_t>() : 0));
}

std::shared_ptr<Keyboard> KeyboardManager::LuaGetKeyboardByID(sol::object id) noexcept
{
	return std::dynamic_pointer_cast<Keyboard>(GetKeyboardByID(id.is<KeyboardID>() ? id.as<KeyboardID>() : 0));
}

sol::table KeyboardManager::LuaListKeyboards() noexcept
{
	sol::table result = GetScriptEngine().CreateTable(_keyboardList.size(), 0);
	for (std::size_t index = 0; index < _keyboardList.size(); ++index)
	{
		result[index + 1] = _keyboardList[index];
	}

	return result;
}
