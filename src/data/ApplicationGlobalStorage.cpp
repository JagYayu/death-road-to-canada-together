#include "data/ApplicationGlobalStorage.hpp"

#include "data/ApplicationGlobalStorage.hpp"
#include "data/GlobalStorageLocation.hpp"
#include "data/GlobalStorageManager.hpp"
#include "util/Log.hpp"

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
