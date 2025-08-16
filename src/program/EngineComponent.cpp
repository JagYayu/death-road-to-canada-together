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

#include "program/EngineComponent.hpp"

using namespace tudov;

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

void IEngineComponent::ProvideLuaAPI(ILuaAPI &luaAPI) noexcept
{
}
