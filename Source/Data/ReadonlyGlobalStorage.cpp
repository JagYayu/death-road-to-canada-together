/**
 * @file data/ReadonlyGlobalStorage.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Data/ReadonlyGlobalStorage.hpp"

#include "Data/GlobalStorage.hpp"
#include "Data/GlobalStorageLocation.hpp"
#include "Data/GlobalStorageManager.hpp"

using namespace tudov;

ReadonlyGlobalStorage::ReadonlyGlobalStorage(GlobalStorageManager &globalStorageManager) noexcept
    : GlobalStorage(globalStorageManager)
{
	// GlobalStorage::_sdlStorage =
}

IGlobalStorageManager &ReadonlyGlobalStorage::GetGlobalStorageManager() noexcept
{
	return _globalStorageManager;
}

Log &ReadonlyGlobalStorage::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(ReadonlyGlobalStorage));
}

bool ReadonlyGlobalStorage::CanRead() noexcept
{
	return true;
}

bool ReadonlyGlobalStorage::CanWrite() noexcept
{
	return false;
}

// Log &ReadonlyGlobalStorage::GetLog() noexcept
// {
// 	return *Log::Get(TE_NAMEOF(ReadonlyGlobalStorage));
// }

constexpr EGlobalStorageLocation ReadonlyGlobalStorage::GetLocation() const noexcept
{
	return EGlobalStorageLocation::Readonly;
}
