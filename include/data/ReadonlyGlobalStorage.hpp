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
		~ReadonlyGlobalStorage() noexcept override = default;

		IGlobalStorageManager &GetGlobalStorageManager() noexcept override;
		Log &GetLog() noexcept override;

		bool CanRead() noexcept override;
		bool CanWrite() noexcept override;

		EGlobalStorageLocation GetLocation() const noexcept override;
	};
} // namespace tudov
