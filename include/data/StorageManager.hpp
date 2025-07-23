#pragma once

#include "Constants.hpp"
#include "program/EngineComponent.hpp"

#include <memory>
#include <string_view>
#include <unordered_map>

namespace tudov
{
	struct IApplicationStorage;
	struct IReadonlyStorage;
	struct IUserStorage;

	struct IStorageManager : public IEngineComponent
	{
		static constexpr decltype(auto) RootUser = "";
		static constexpr decltype(auto) UserDirectoryPrefix = Constants::AppUserDirectoryPrefix;

		virtual ~IStorageManager() noexcept override = default;

		virtual IApplicationStorage &GetAppStorage() noexcept = 0;
		virtual IReadonlyStorage &GetReadonlyStorage() noexcept = 0;
		virtual IUserStorage &GetUserStorage(std::string_view username = RootUser) noexcept = 0;
		virtual bool HasUser(std::string_view username = RootUser) noexcept = 0;
		virtual bool DeleteUser(std::string_view username = RootUser) noexcept = 0;
	};

	class ApplicationStorage;
	class ReadonlyStorage;
	class UserStorage;

	class StorageManager : public IStorageManager
	{
	  protected:
		Context &_context;
		std::shared_ptr<ApplicationStorage> _appStorage;
		std::shared_ptr<ReadonlyStorage> _readonlyStorage;
		std::unordered_map<std::string_view, std::shared_ptr<UserStorage>> _userStorages;

	  public:
		explicit StorageManager(Context &context) noexcept;
		~StorageManager() noexcept override = default;

		Context &GetContext() noexcept override;

		IApplicationStorage &GetAppStorage() noexcept override;
		IReadonlyStorage &GetReadonlyStorage() noexcept override;
		IUserStorage &GetUserStorage(std::string_view user = RootUser) noexcept override;
		bool HasUser(std::string_view user = RootUser) noexcept override;
		bool DeleteUser(std::string_view user = RootUser) noexcept override;
	};
} // namespace tudov
