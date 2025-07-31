#pragma once

#include "Storage.hpp"
#include "util/Log.hpp"
#include <string_view>

struct SDL_Storage;

namespace tudov
{
	enum class EGlobalStorageLocation;
	struct IGlobalStorageManager;

	/**
	 * Abstract Class
	 */
	class GlobalStorage : public IStorage, public ILogProvider
	{
	  protected:
		IGlobalStorageManager &_globalStorageManager;
		SDL_Storage *_sdlStorage;

	  public:
		explicit GlobalStorage(IGlobalStorageManager &globalStorageManager) noexcept;
		~GlobalStorage() noexcept override;

		bool IsReady() noexcept override;

		bool CanRead() noexcept override;

		EStorageEnumerationResult EnumerateDirectory(std::string_view path, const EnumerationCallbackFunction<> &callback, void *callbackArgs = nullptr) noexcept override;

		bool Exists(std::string_view path) noexcept override;

		// PathInfo GetPathInfo(std::string_view path) noexcept override;

		std::uint64_t GetPathSize(std::string_view filePath) noexcept override;
		EPathType GetPathType(std::string_view path) noexcept override;

		std::vector<std::byte> ReadFileToBytes(std::string_view filePath) override;
		// std::string ReadFileToString(std::string_view filePath) override;

		virtual IGlobalStorageManager &GetGlobalStorageManager() noexcept = 0;

		virtual EGlobalStorageLocation GetLocation() const noexcept = 0;

		inline const IGlobalStorageManager &GetGlobalStorageManager() const noexcept
		{
			return const_cast<GlobalStorage *>(this)->GetGlobalStorageManager();
		}
	};
} // namespace tudov
