/**
 * @file resource/GameScripts.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "scripts/GameScripts.hpp"

using namespace tudov;

GameScripts::GameScripts(Context &context) noexcept
    : _context(context)
{
}

Context &GameScripts::GetContext() noexcept
{
	return _context;
}

void GameScripts::Initialize() noexcept
{
}

void GameScripts::Deinitialize() noexcept
{
}
