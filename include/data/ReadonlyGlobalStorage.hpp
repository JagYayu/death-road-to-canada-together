/**
 * @file data/ReadonlyGlobalStorage.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "GlobalStorage.hpp"
#include "Storage.hpp"

namespace tudov
{
	struct GlobalStorageManager;

	class ReadonlyGlobalStorage : public GlobalStorage
	{
	  public:
		explicit ReadonlyGlobalStorage(GlobalStorageManager &globalStorageManager) noexcept;
		explicit ReadonlyGlobalStorage(const ReadonlyGlobalStorage &) noexcept = default;
		explicit ReadonlyGlobalStorage(ReadonlyGlobalStorage &&) noexcept = default;
		ReadonlyGlobalStorage &operator=(const ReadonlyGlobalStorage &) noexcept = delete;
		ReadonlyGlobalStorage &operator=(ReadonlyGlobalStorage &&) noexcept = delete;
		~ReadonlyGlobalStorage() noexcept override = default;

		IGlobalStorageManager &GetGlobalStorageManager() noexcept override;
		Log &GetLog() noexcept override;

		bool CanRead() noexcept override;
		bool CanWrite() noexcept override;

		constexpr EGlobalStorageLocation GetLocation() const noexcept override;
	};
} // namespace tudov
