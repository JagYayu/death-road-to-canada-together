#pragma once

#include "GlobalStorage.hpp"
#include "Storage.hpp"

#include <string>

namespace tudov
{
	struct GlobalStorageManager;

	class UserGlobalStorage : public GlobalStorage
	{
	  protected:
		std::string _username;

	  public:
		explicit UserGlobalStorage(GlobalStorageManager &globalStorageManager, std::string_view username) noexcept;
		~UserGlobalStorage() noexcept override = default;

		std::string_view GetUsername() noexcept;
		IGlobalStorageManager &GetGlobalStorageManager() noexcept override;
		Log &GetLog() noexcept override;

		bool CanRead() noexcept override;
		bool CanWrite() noexcept override;

		EGlobalStorageLocation GetLocation() const noexcept override;
	};
} // namespace tudov
