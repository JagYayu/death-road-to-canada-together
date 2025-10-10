/**
 * @file data/GlobalStorageManager.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Data/GlobalStorageManager.hpp"

#include "Data/ApplicationGlobalStorage.hpp"
#include "Data/ReadonlyGlobalStorage.hpp"
#include "Data/UserGlobalStorage.hpp"

#include <memory>

using namespace tudov;

GlobalStorageManager::GlobalStorageManager() noexcept
{
}

// Context &GlobalStorageManager::GetContext() noexcept
// {
// 	return _context;
// }

GlobalStorage &GlobalStorageManager::GetApplicationStorage() noexcept
{
	if (!_application) [[unlikely]]
	{
		_application = std::make_shared<ApplicationGlobalStorage>(*this);
	}
	return *_application;
}

GlobalStorage &GlobalStorageManager::GetReadonlyStorage() noexcept
{
	if (!_readonly) [[unlikely]]
	{
		_readonly = std::make_shared<ReadonlyGlobalStorage>(*this);
	}
	return *_readonly;
}

GlobalStorage &GlobalStorageManager::GetUserStorage(std::string_view username) noexcept
{
	auto it = _users.find(username);
	if (it == _users.end()) [[unlikely]]
	{
		auto &&userGlobalStorage = std::make_shared<UserGlobalStorage>(*this, username);
		it = _users.try_emplace(userGlobalStorage->GetUsername(), userGlobalStorage).first;
	}
	return *it->second;
}

bool GlobalStorageManager::HasUserStorage(std::string_view username) noexcept
{
	// TODO
	return true;
}

bool GlobalStorageManager::DeleteUserStorage(std::string_view username) noexcept
{
	return _users.erase(username);
}
