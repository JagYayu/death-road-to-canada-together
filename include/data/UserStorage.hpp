#pragma once

#include "Storage.hpp"
#include "program/EngineComponent.hpp"

#include <string>

namespace tudov
{
	struct IUserStorage : public virtual IStorage
	{
		virtual ~IUserStorage() noexcept override = default;

		virtual std::string_view GetUsername() noexcept = 0;
	};

	struct StorageManager;

	class UserStorage : public Storage, public IUserStorage
	{
	  protected:
		std::string _username;
		StorageManager &_storageManager;

	  public:
		explicit UserStorage(StorageManager &storageManager, std::string_view username) noexcept;
		~UserStorage() noexcept override = default;

		std::string_view GetUsername() noexcept override;
		IStorageManager &GetStorageManager() noexcept override;
		Log &GetLog() noexcept override;

		bool CanRead() noexcept override;
		bool CanWrite() noexcept override;
	};
} // namespace tudov
