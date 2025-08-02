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
		~ApplicationGlobalStorage() noexcept override= default;

		IGlobalStorageManager &GetGlobalStorageManager() noexcept override;
		Log &GetLog() noexcept override;

		bool CanRead() noexcept override;
		bool CanWrite() noexcept override;

		EGlobalStorageLocation GetLocation() const noexcept override;
	};
} // namespace tudov
