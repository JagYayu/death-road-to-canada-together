#include "ApplicationStorage.hpp"

#include "StorageManager.hpp"

using namespace tudov;

ApplicationStorage::ApplicationStorage(StorageManager &storageManager) noexcept
    : _storageManager(storageManager)
{
}

IStorageManager &ApplicationStorage::GetStorageManager() noexcept
{
	return _storageManager;
}

bool ApplicationStorage::CanRead() noexcept
{
	return true;
}

bool ApplicationStorage::CanWrite() noexcept
{
	return false;
}
