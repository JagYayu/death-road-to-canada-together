/**
 * @file data/AssetsManager.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "event/DelegateEvent.hpp"
#include "Program/EngineComponent.hpp"

#include "System/Log.hpp"
#include "util/Version.hpp"

#include <string>
#include <vector>

namespace tudov
{
	struct IAssetsManager : public IEngineComponent
	{
		struct PackInfo
		{
			std::string path;
			std::string name;
			Version version;
		};

		~IAssetsManager() noexcept override = default;

		virtual const std::vector<PackInfo> &GetFounded() const noexcept = 0;
		virtual const std::vector<PackInfo> &GetLoaded() const noexcept = 0;
	};

	enum class EFileChangeType : int;
	enum class EPathType : int;
	class FileSystemWatch;
	class GlobalStorage;

	class AssetsManager : public IAssetsManager, private ILogProvider
	{
	  protected:
		Context &_context;
		std::vector<PackInfo> _founded;
		std::vector<PackInfo> _loaded;
		std::unique_ptr<FileSystemWatch> _developerDirectoryWatch;
		DelegateEvent<const std::filesystem::path &, EFileChangeType> _developerFilesTrigger;

	  public:
		explicit AssetsManager(Context &context) noexcept;
		explicit AssetsManager(const AssetsManager &) noexcept = delete;
		explicit AssetsManager(AssetsManager &&) noexcept = delete;
		AssetsManager &operator=(const AssetsManager &) noexcept = delete;
		AssetsManager &operator=(AssetsManager &&) noexcept = delete;
		~AssetsManager() noexcept override;

		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;
		void Initialize() noexcept override;
		void Deinitialize() noexcept override;

		DelegateEvent<const std::filesystem::path &, EFileChangeType> &GetOnDeveloperFilesTrigger() noexcept;

		const std::vector<PackInfo> &GetFounded() const noexcept override;
		const std::vector<PackInfo> &GetLoaded() const noexcept override;

		inline const DelegateEvent<const std::filesystem::path &, EFileChangeType> &GetOnDeveloperFilesTrigger() const noexcept
		{
			return const_cast<AssetsManager *>(this)->GetOnDeveloperFilesTrigger();
		}

	  private:
		void LoadAssetsFromPackageFiles() noexcept;
		void LoadAssetsFromDeveloperDirectory() noexcept;
		void LoadAssetsFromExternalDirectories() noexcept;
		void DeveloperDirectoryWatchCallback(const std::filesystem::path &filePath, EPathType pathType, EFileChangeType type);
		std::vector<std::vector<std::byte>> CollectPackageFileBytes() noexcept;
	};
} // namespace tudov
