#include "data/ReadonlyStorage.hpp"

#include "data/StorageManager.hpp"

using namespace tudov;

ReadonlyStorage::ReadonlyStorage(StorageManager &storageManager) noexcept
    : _storageManager(storageManager)
{
}

IStorageManager &ReadonlyStorage::GetStorageManager() noexcept
{
	return _storageManager;
}

Log &ReadonlyStorage::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(ReadonlyStorage));
}

bool ReadonlyStorage::CanRead() noexcept
{
	return true;
}

bool ReadonlyStorage::CanWrite() noexcept
{
	return false;
}

// Log &ReadonlyStorage::GetLog() noexcept
// {
// 	return *Log::Get(TE_NAMEOF(ReadonlyStorage));
// }
