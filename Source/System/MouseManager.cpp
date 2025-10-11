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

#include "System/MouseManager.hpp"

#include "System/Mouse.hpp"

#include "SDL3/SDL_mouse.h"
#include "Util/Micros.hpp"

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
	std::int32_t count;
	SDL_MouseID *mouseIDs = SDL_GetMice(&count);

	_mouseList.reserve(count);
	_mouseMap.reserve(count);

	AddMouse(_primaryMouse);

	for (std::int32_t index = 0; index < count; ++index)
	{
		auto id = mouseIDs[index];
		AddMouse(std::make_shared<Mouse>(_context, id));
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

bool MouseManager::HandleEvent(AppEvent &appEvent) noexcept
{
	for (std::shared_ptr<Mouse> &mouse : _mouseList)
	{
		if (mouse->HandleEvent(appEvent))
		{
			return true;
		}
	}
	return false;
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
