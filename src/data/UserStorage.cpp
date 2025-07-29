#include "data/UserStorage.hpp"

#include "data/StorageManager.hpp"

using namespace tudov;

UserStorage::UserStorage(StorageManager &storageManager, std::string_view username) noexcept
    : _storageManager(storageManager),
      _username(username)
{
}

std::string_view UserStorage::GetUsername() noexcept
{
	return _username;
}

IStorageManager &UserStorage::GetStorageManager() noexcept
{
	return _storageManager;
}

Log &UserStorage::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(UserStorage));
}

bool UserStorage::CanRead() noexcept
{
	return true;
}

bool UserStorage::CanWrite() noexcept
{
	return true;
}
