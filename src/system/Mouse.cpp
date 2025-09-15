/**
 * @file program/Mouse.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "system/Mouse.hpp"

using namespace tudov;

Mouse::Mouse(MouseID mouseID) noexcept
    : _mouseID(mouseID)
{
}

MouseID Mouse::GetMouseID() noexcept
{
	return _mouseID;
}
