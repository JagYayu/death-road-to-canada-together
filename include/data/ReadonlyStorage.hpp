#pragma once

#include "Storage.hpp"

namespace tudov
{
	struct IReadonlyStorage : public virtual IStorage
	{
		virtual ~IReadonlyStorage() noexcept = default;
	};

	class StorageManager;

	class ReadonlyStorage : public Storage, public IReadonlyStorage
	{
	  protected:
		StorageManager &_storageManager;

	  public:
		explicit ReadonlyStorage(StorageManager &storageManager) noexcept;
		~ReadonlyStorage() noexcept override = default;

		IStorageManager &GetStorageManager() noexcept override;
		Log &GetLog() noexcept override;

		bool CanRead() noexcept override;
		bool CanWrite() noexcept override;
	};
} // namespace tudov
