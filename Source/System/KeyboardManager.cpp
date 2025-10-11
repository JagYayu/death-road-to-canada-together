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

#include "System/KeyboardManager.hpp"

#include "Event/AppEvent.hpp"
#include "Event/CoreEvents.hpp"
#include "Event/CoreEventsData.hpp"
#include "Event/EventManager.hpp"
#include "Event/RuntimeEvent.hpp"
#include "Mod/ScriptEngine.hpp"
#include "Program/WindowManager.hpp"
#include "System/Keyboard.hpp"
#include "System/LogMicros.hpp"
#include "Util/Definitions.hpp"
#include "Util/Micros.hpp"

#include "SDL3/SDL_keyboard.h"

#include <memory>

using namespace tudov;

KeyboardManager::KeyboardManager(Context &context) noexcept
    : _context(context),
      _primaryKeyboard(std::make_shared<PrimaryKeyboard>(context))
{
	std::int32_t count;
	SDL_KeyboardID *keyboardIDs = SDL_GetKeyboards(&count);

	_keyboardList.reserve(count);
	_keyboardMap.reserve(count);

	AddKeyboard(_primaryKeyboard);

	for (std::int32_t index = 0; index < count; ++index)
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

	TE_DEBUG("Added keyboard {}", keyboard->GetKeyboardID());
}

// void KeyboardManager::RemoveKeyboard(const std::shared_ptr<Keyboard> &keyboard) noexcept
// {
// 	std::remove_if(_keyboardList, keyboard, []()
// 	{
// 		return 1;
// 	});
// }

// void KeyboardManager::RemoveKeyboardByID(KeyboardID &keyboard) noexcept
// {
// }

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
	EventHandleFunction added{[this](sol::object e, const EventHandleKey &key)
	{
		OnKeyboardDeviceAdded(e, key);
	}};

	// EventHandleFunction removed{[this](sol::object e, const EventHandleKey &key)
	// {
	// 	OnKeyboardDeviceRemoved(e, key);
	// }};

	try
	{
		ICoreEvents &coreEvents = GetEventManager().GetCoreEvents();

		coreEvents.KeyboardAdded().Add(added, TE_NAMEOF(KeyboardManager::OnKeyboardDeviceAdded), RuntimeEvent::DefaultOrders[0]);
		// coreEvents.KeyboardRemoved().Add(removed, TE_NAMEOF(KeyboardManager::OnKeyboardDeviceAdded), RuntimeEvent::DefaultOrders[0]);
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

// void KeyboardManager::OnKeyboardDeviceRemoved(sol::object e, const EventHandleKey &key) noexcept
// {
// 	auto data = CoreEventData::Extract<EventKeyboardDeviceData>(e);
// 	if (data == nullptr) [[unlikely]]
// 	{
// 		TE_WARN("Failed to add keyboard device!");

// 		return;
// 	}
// }

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

std::shared_ptr<IKeyboard> KeyboardManager::GetIKeyboardAt(std::int32_t index) noexcept
{
	if (index >= 0 && index < _keyboardList.size())
	{
		return _keyboardList.at(index);
	}
	return nullptr;
}

std::shared_ptr<IKeyboard> KeyboardManager::GetIKeyboardByID(KeyboardID id) noexcept
{
	auto it = _keyboardMap.find(id);
	if (it != _keyboardMap.end())
	{
		return it->second;
	}
	return nullptr;
}

std::shared_ptr<IKeyboard> KeyboardManager::GetPrimaryIKeyboard() noexcept
{
	return _primaryKeyboard;
}

std::vector<std::shared_ptr<IKeyboard>> *KeyboardManager::GetIKeyboards() noexcept
{
	return &_keyboards;
}

std::shared_ptr<Keyboard> KeyboardManager::LuaGetKeyboardAt(sol::object index) noexcept
{
	return std::dynamic_pointer_cast<Keyboard>(GetIKeyboardAt(index.is<std::int32_t>() ? index.as<std::int32_t>() : 0));
}

std::shared_ptr<Keyboard> KeyboardManager::LuaGetKeyboardByID(sol::object id) noexcept
{
	return std::dynamic_pointer_cast<Keyboard>(GetIKeyboardByID(id.is<KeyboardID>() ? id.as<KeyboardID>() : 0));
}

std::shared_ptr<Keyboard> KeyboardManager::LuaGetPrimaryKeyboard() noexcept
{
	return _primaryKeyboard;
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
