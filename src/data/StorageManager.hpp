#pragma once

#include "AppStorage.hpp"
#include "ReadonlyStorage.hpp"
#include "Storage.hpp"
#include "UserStorage.hpp"
#include "program/EngineComponent.hpp"

#include <memory>
#include <string_view>

namespace tudov
{
	struct IReadonlyStorage;
	struct IUserStorage;

	struct IStorageManager : public IStorage, public IEngineComponent
	{
		virtual ~IStorageManager() noexcept override = default;

		virtual std::shared_ptr<IAppStorage> OpenAppStorage() noexcept = 0;
		virtual std::shared_ptr<IReadonlyStorage> OpenReadonlyStorage() noexcept = 0;
		virtual std::shared_ptr<IUserStorage> OpenUserStorage(std::string_view user = "") noexcept = 0;
	};

	class StorageManager : public IStorageManager
	{
	  public:
	};
} // namespace tudov
