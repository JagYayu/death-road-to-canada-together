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

#include "data/ReadonlyGlobalStorage.hpp"

#include "data/GlobalStorage.hpp"
#include "data/GlobalStorageLocation.hpp"
#include "data/GlobalStorageManager.hpp"

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
