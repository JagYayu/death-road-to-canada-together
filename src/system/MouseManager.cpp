/**
 * @file program/MouseManager.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "system/MouseManager.hpp"

#include "system/Mouse.hpp"

#include "SDL3/SDL_mouse.h"
#include "util/Micros.hpp"

#include <format>
#include <memory>
#include <stdexcept>

using namespace tudov;

MouseManager::MouseManager(Context &context) noexcept
    : _context(context),
      _primaryMouse(std::make_shared<PrimaryMouse>(context)),
      _mouseList(),
      _mouseMap()
{
	try
	{
		AddMouse(_primaryMouse);
	}
	catch (const std::runtime_error &e)
	{
		Warn("Error adding primary mouse: {}", e.what());

		_primaryMouse = nullptr;
	}
}

Context &MouseManager::GetContext() noexcept
{
	return _context;
}

Log &MouseManager::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(MouseManager));
}

bool MouseManager::IsMouseAvailable(MouseID id) const noexcept
{
	return SDL_HasMouse();
}

std::shared_ptr<IMouse> MouseManager::GetMouseAt(std::int32_t index) noexcept
{
	if (index >= 0 && index < _mouseList.size())
	{
		return std::dynamic_pointer_cast<IMouse>(_mouseList.at(index));
	}
	return nullptr;
}

std::shared_ptr<IMouse> MouseManager::GetMouseByID(MouseID id) noexcept
{
	auto it = _mouseMap.find(id);
	if (it != _mouseMap.end())
	{
		return std::dynamic_pointer_cast<IMouse>(it->second);
	}
	return nullptr;
}

std::shared_ptr<IMouse> MouseManager::GetPrimaryMouse() noexcept
{
	return _primaryMouse;
}

void MouseManager::AddMouse(std::shared_ptr<Mouse> mouse)
{
	if (!_mouseMap.try_emplace(mouse->GetMouseID(), mouse).second)
	{
		throw std::runtime_error(std::format("Mouse id {} duplicated!", mouse->GetMouseID()));
	}
	_mouseList.emplace_back(mouse);
}

std::shared_ptr<Mouse> MouseManager::LuaGetMouseAt(std::int32_t index) noexcept
{
	return std::dynamic_pointer_cast<Mouse>(GetMouseAt(index));
}

std::shared_ptr<Mouse> MouseManager::LuaGetMouseByID(MouseID id) noexcept
{
	return std::dynamic_pointer_cast<Mouse>(GetMouseByID(id));
}

std::shared_ptr<Mouse> MouseManager::LuaGetPrimaryMouse() noexcept
{
	return std::dynamic_pointer_cast<Mouse>(_primaryMouse);
}
