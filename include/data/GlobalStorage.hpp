#pragma once

#include "Storage.hpp"
#include "util/Log.hpp"

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

		EStorageIterationResult ForeachDirectory(const std::filesystem::path &path, const EnumerationCallbackFunction<> &callback, void *callbackArgs = nullptr) noexcept override;

		bool Exists(const std::filesystem::path &path) noexcept override;

		// PathInfo GetPathInfo(const std::filesystem::path & path) noexcept override;

		std::uint64_t GetPathSize(const std::filesystem::path &filePath) noexcept override;
		EPathType GetPathType(const std::filesystem::path &path) noexcept override;

		std::vector<std::byte> ReadFileToBytes(const std::filesystem::path &filePath) override;
		// std::string ReadFileToString(const std::filesystem::path & filePath) override;

		virtual IGlobalStorageManager &GetGlobalStorageManager() noexcept = 0;

		virtual EGlobalStorageLocation GetLocation() const noexcept = 0;

		inline const IGlobalStorageManager &GetGlobalStorageManager() const noexcept
		{
			return const_cast<GlobalStorage *>(this)->GetGlobalStorageManager();
		}
	};
} // namespace tudov
