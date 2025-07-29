#pragma once

#include "Storage.hpp"

struct SDL_Storage;

namespace tudov
{
	struct IApplicationStorage : public virtual IStorage
	{
		virtual ~IApplicationStorage() noexcept = default;
	};

	class StorageManager;

	class ApplicationStorage : public Storage, public IApplicationStorage
	{
	  protected:
		StorageManager &_storageManager;

	  public:
		explicit ApplicationStorage(StorageManager &storageManager) noexcept;
		~ApplicationStorage() noexcept = default;

		IStorageManager &GetStorageManager() noexcept override;
		Log &GetLog() noexcept override;

		bool CanRead() noexcept override;
		bool CanWrite() noexcept override;
	};
} // namespace tudov
