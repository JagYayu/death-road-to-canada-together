#pragma once

#include "Constants.hpp"
#include "program/EngineComponent.hpp"

#include <memory>
#include <string_view>
#include <unordered_map>

namespace tudov
{
	class GlobalStorage;

	struct IGlobalStorageManager : public IEngineComponent
	{
		static constexpr decltype(auto) RootUser = "";
		static constexpr decltype(auto) UserDirectoryPrefix = Constants::AppUserDirectoryPrefix;

		virtual ~IGlobalStorageManager() noexcept override = default;

		virtual GlobalStorage &GetApplicationStorage() noexcept = 0;
		virtual GlobalStorage &GetReadonlyStorage() noexcept = 0;
		virtual GlobalStorage &GetUserStorage(std::string_view username = RootUser) noexcept = 0;
		virtual bool HasUser(std::string_view username = RootUser) noexcept = 0;
		virtual bool DeleteUser(std::string_view username = RootUser) noexcept = 0;
	};

	class GlobalStorageManager : public IGlobalStorageManager
	{
	  protected:
		Context &_context;
		std::shared_ptr<GlobalStorage> _application;
		std::shared_ptr<GlobalStorage> _readonly;
		std::unordered_map<std::string_view, std::shared_ptr<GlobalStorage>> _users;

	  public:
		explicit GlobalStorageManager(Context &context) noexcept;
		~GlobalStorageManager() noexcept override = default;

		Context &GetContext() noexcept override;

		GlobalStorage &GetApplicationStorage() noexcept override;
		GlobalStorage &GetReadonlyStorage() noexcept override;
		GlobalStorage &GetUserStorage(std::string_view user = RootUser) noexcept override;
		bool HasUser(std::string_view user = RootUser) noexcept override;
		bool DeleteUser(std::string_view user = RootUser) noexcept override;
	};
} // namespace tudov
