#pragma once

#include "Storage.hpp"

namespace tudov
{
	struct IReadonlyStorage : public IStorage
	{
		virtual ~IReadonlyStorage() noexcept = default;
	};

	class StorageManager;

	class ReadonlyStorage : public IReadonlyStorage
	{
	  protected:
		StorageManager &_storageManager;

	  public:
		explicit ReadonlyStorage(StorageManager &storageManager) noexcept;
		~ReadonlyStorage() noexcept override = default;

		IStorageManager &GetStorageManager() noexcept override;

		bool CanRead() noexcept override;
		bool CanWrite() noexcept override;
	};
} // namespace tudov
