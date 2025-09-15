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

using namespace tudov;

MouseManager::MouseManager(Context &context) noexcept
    : _context(context)
{
}

Context &MouseManager::GetContext() noexcept
{
	return _context;
}

std::shared_ptr<Mouse> MouseManager::GetMouseAt(std::int32_t index) noexcept
{
	if (index >= 0 && index < _mouseList.size())
	{
		return _mouseList.at(index);
	}
	return nullptr;
}

std::shared_ptr<Mouse> MouseManager::GetMouseByID(MouseID id) noexcept
{
	auto it = _mouseMap.find(id);
	if (it != _mouseMap.end())
	{
		return it->second;
	}
	return nullptr;
}
