/**
 * @file data/GlobalStorageManager.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Constants.hpp"

#include <memory>
#include <string_view>
#include <unordered_map>

namespace tudov
{
	class GlobalStorage;

	struct IGlobalStorageManager
	{
		static constexpr decltype(auto) RootUser = "";
		static constexpr decltype(auto) UserDirectoryPrefix = Constants::DataUserDirectoryPrefix;

		virtual ~IGlobalStorageManager() noexcept = default;

		virtual GlobalStorage &GetApplicationStorage() noexcept = 0;

		virtual GlobalStorage &GetReadonlyStorage() noexcept = 0;

		virtual GlobalStorage &GetUserStorage(std::string_view username = RootUser) noexcept = 0;

		virtual bool HasUserStorage(std::string_view username = RootUser) noexcept = 0;

		virtual bool DeleteUserStorage(std::string_view username = RootUser) noexcept = 0;
	};

	class GlobalStorageManager : public IGlobalStorageManager
	{
	  protected:
		std::shared_ptr<GlobalStorage> _application;
		std::shared_ptr<GlobalStorage> _readonly;
		std::unordered_map<std::string_view, std::shared_ptr<GlobalStorage>> _users;

	  public:
		explicit GlobalStorageManager() noexcept;
		~GlobalStorageManager() noexcept override = default;

		// Context &GetContext() noexcept override;

		GlobalStorage &GetApplicationStorage() noexcept override;
		GlobalStorage &GetReadonlyStorage() noexcept override;
		GlobalStorage &GetUserStorage(std::string_view user = RootUser) noexcept override;
		bool HasUserStorage(std::string_view user = RootUser) noexcept override;
		bool DeleteUserStorage(std::string_view user = RootUser) noexcept override;
	};
} // namespace tudov
