/**
 * @file program/EngineComponent.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Program/EngineComponent.hpp"

#include "event/AppEvent.hpp"

using namespace tudov;

std::int32_t IEngineComponent::GetSortingSequence() noexcept
{
	return 0;
}

void IEngineComponent::PreInitialize() noexcept
{
}

void IEngineComponent::Initialize() noexcept
{
}

void IEngineComponent::Deinitialize() noexcept
{
}

void IEngineComponent::PostDeinitialize() noexcept
{
}

void IEngineComponent::ProvideLuaBindings(ILuaBindings &luaBindings) noexcept
{
}

bool IEngineComponent::HandleEvent(AppEvent &appEvent) noexcept
{
	return false;
}

void IEngineComponent::ProcessTick() noexcept
{
}

void IEngineComponent::ProcessRender() noexcept
{
}

void IEngineComponent::ProcessLoad() noexcept
{
}
