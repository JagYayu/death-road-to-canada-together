/**
 * @file data/UserGlobalStorage.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "data/UserGlobalStorage.hpp"

#include "data/GlobalStorageLocation.hpp"
#include "data/GlobalStorageManager.hpp"

using namespace tudov;

UserGlobalStorage::UserGlobalStorage(GlobalStorageManager &globalStorageManager, std::string_view username) noexcept
    : GlobalStorage(globalStorageManager),
      _username(username)
{
}

std::string_view UserGlobalStorage::GetUsername() noexcept
{
	return _username;
}

IGlobalStorageManager &UserGlobalStorage::GetGlobalStorageManager() noexcept
{
	return _globalStorageManager;
}

Log &UserGlobalStorage::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(UserGlobalStorage));
}

bool UserGlobalStorage::CanRead() noexcept
{
	return true;
}

bool UserGlobalStorage::CanWrite() noexcept
{
	return true;
}

constexpr EGlobalStorageLocation UserGlobalStorage::GetLocation() const noexcept
{
	return EGlobalStorageLocation::User;
}
