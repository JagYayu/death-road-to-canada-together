#pragma once

#include "Storage.hpp"

namespace tudov
{
	struct IApplicationStorage : public IStorage
	{
		virtual ~IApplicationStorage() noexcept = default;
	};

	class StorageManager;

	class ApplicationStorage : public IApplicationStorage
	{
	  protected:
		StorageManager &_storageManager;

	  public:
		explicit ApplicationStorage(StorageManager &storageManager) noexcept;
		~ApplicationStorage() noexcept override = default;

		IStorageManager &GetStorageManager() noexcept override;

		bool CanRead() noexcept override;
		bool CanWrite() noexcept override;
	};
} // namespace tudov
