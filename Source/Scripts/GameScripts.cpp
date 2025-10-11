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

#include "Scripts/GameScripts.hpp"
#include "Event/EventManager.hpp"

using namespace tudov;

void IGameScripts::Initialize() noexcept
{
	RegisterEvents(GetEventManager());
}

GameScripts::GameScripts(Context &context) noexcept
    : _context(context)
{
}

Context &GameScripts::GetContext() noexcept
{
	return _context;
}

Log &GameScripts::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(GameScripts));
}

void GameScripts::Initialize() noexcept
{
}

void GameScripts::Deinitialize() noexcept
{
}

void GameScripts::RegisterEvents(IEventManager &eventManager) noexcept
{
}

void GameScripts::ProvideLuaBindings(sol::state &lua, Context &context)
{
}
