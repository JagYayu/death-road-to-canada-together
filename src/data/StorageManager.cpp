#include "data/StorageManager.hpp"

#include "data/ApplicationStorage.hpp"
#include "data/ReadonlyStorage.hpp"
#include "data/UserStorage.hpp"

#include <memory>

using namespace tudov;

StorageManager::StorageManager(Context &context) noexcept
    : _context(context)
{
}

Context &StorageManager::GetContext() noexcept
{
	return _context;
}

IApplicationStorage &StorageManager::GetAppStorage() noexcept
{
	if (!_appStorage) [[unlikely]]
	{
		_appStorage = std::make_shared<ApplicationStorage>(*this);
	}
	return *_appStorage;
}

IReadonlyStorage &StorageManager::GetReadonlyStorage() noexcept
{
	if (!_readonlyStorage) [[unlikely]]
	{
		_readonlyStorage = std::make_shared<ReadonlyStorage>(*this);
	}
	return *_readonlyStorage;
}

IUserStorage &StorageManager::GetUserStorage(std::string_view username) noexcept
{
	auto &&it = _userStorages.find(username);
	if (it == _userStorages.end()) [[unlikely]]
	{
		auto &&userStorage = std::make_shared<UserStorage>(*this, username);
		it = _userStorages.try_emplace(userStorage->GetUsername(), userStorage).first;
	}
	return *it->second;
}

bool StorageManager::HasUser(std::string_view username) noexcept
{
	// TODO
	return true;
}

bool StorageManager::DeleteUser(std::string_view username) noexcept
{
	return _userStorages.erase(username);
}
