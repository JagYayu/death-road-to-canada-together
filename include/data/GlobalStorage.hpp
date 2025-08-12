#pragma once

#include "HierarchyElement.hpp"
#include "Storage.hpp"
#include "util/Log.hpp"

#include <cstdint>

struct SDL_Storage;

namespace tudov
{
	enum class EGlobalStorageLocation : std::uint8_t;
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

		EHierarchyIterationResult Foreach(const Path &path, const EnumerationCallbackFunction<> &callback, void *callbackArgs = nullptr) noexcept override;

		EHierarchyElement Check(const Path &path) noexcept override;

		PathInfo GetPathInfo(const Path &path) noexcept override;

		std::uint64_t GetPathSize(const Path &filePath) noexcept override;
		EPathType GetPathType(const Path &path) noexcept override;

		std::vector<std::byte> ReadFileToBytes(const Path &filePath) override;

		virtual IGlobalStorageManager &GetGlobalStorageManager() noexcept = 0;

		virtual EGlobalStorageLocation GetLocation() const noexcept = 0;

		inline const IGlobalStorageManager &GetGlobalStorageManager() const noexcept
		{
			return const_cast<GlobalStorage *>(this)->GetGlobalStorageManager();
		}
	};
} // namespace tudov
