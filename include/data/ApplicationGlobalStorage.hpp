/**
 * @file data/ApplicationGlobalStorage.hpp
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
	class GlobalStorageManager;

	class ApplicationGlobalStorage : public GlobalStorage
	{
	  public:
		explicit ApplicationGlobalStorage(GlobalStorageManager &globalStorageManager) noexcept;
		explicit ApplicationGlobalStorage(const ApplicationGlobalStorage &) noexcept = default;
		explicit ApplicationGlobalStorage(ApplicationGlobalStorage &&) noexcept = default;
		ApplicationGlobalStorage &operator=(const ApplicationGlobalStorage &) noexcept = delete;
		ApplicationGlobalStorage &operator=(ApplicationGlobalStorage &&) noexcept = delete;

		IGlobalStorageManager &GetGlobalStorageManager() noexcept override;
		Log &GetLog() noexcept override;

		bool CanRead() noexcept override;
		bool CanWrite() noexcept override;

		constexpr EGlobalStorageLocation GetLocation() const noexcept override;
	};
} // namespace tudov
