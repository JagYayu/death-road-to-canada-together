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
