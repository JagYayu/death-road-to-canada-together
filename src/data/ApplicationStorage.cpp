#include "data/ApplicationStorage.hpp"

#include "data/StorageManager.hpp"
#include "util/Log.hpp"

#include "SDL3/SDL_properties.h"
#include "SDL3/SDL_storage.h"

using namespace tudov;

ApplicationStorage::ApplicationStorage(StorageManager &storageManager) noexcept
    : _storageManager(storageManager)
{
	_storage = SDL_OpenTitleStorage(nullptr, SDL_PropertiesID(0));
}

IStorageManager &ApplicationStorage::GetStorageManager() noexcept
{
	return _storageManager;
}

Log &ApplicationStorage::GetLog() noexcept
{
	return *Log::Get("ApplicationStorage");
}

bool ApplicationStorage::CanRead() noexcept
{
	return true;
}

bool ApplicationStorage::CanWrite() noexcept
{
	return false;
}
