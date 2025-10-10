/**
 * @file data/ApplicationGlobalStorage.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Data/ApplicationGlobalStorage.hpp"

#include "Data/ApplicationGlobalStorage.hpp"
#include "Data/GlobalStorageLocation.hpp"
#include "Data/GlobalStorageManager.hpp"
#include "System/Log.hpp"

#include "SDL3/SDL_properties.h"
#include "SDL3/SDL_storage.h"

using namespace tudov;

ApplicationGlobalStorage::ApplicationGlobalStorage(GlobalStorageManager &globalStorageManager) noexcept
    : GlobalStorage(globalStorageManager)
{
	GlobalStorage::_sdlStorage = SDL_OpenTitleStorage(nullptr, static_cast<SDL_PropertiesID>(0));
}

IGlobalStorageManager &ApplicationGlobalStorage::GetGlobalStorageManager() noexcept
{
	return _globalStorageManager;
}

Log &ApplicationGlobalStorage::GetLog() noexcept
{
	return *Log::Get("ApplicationGlobalStorage");
}

bool ApplicationGlobalStorage::CanRead() noexcept
{
	return true;
}

bool ApplicationGlobalStorage::CanWrite() noexcept
{
	return false;
}

constexpr EGlobalStorageLocation ApplicationGlobalStorage::GetLocation() const noexcept
{
	return EGlobalStorageLocation::Application;
}
