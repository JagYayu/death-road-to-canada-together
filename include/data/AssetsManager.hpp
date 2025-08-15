#pragma once

#include "event/DelegateEvent.hpp"
#include "program/EngineComponent.hpp"

#include "util/Log.hpp"
#include "util/Version.hpp"

#include <memory>
#include <string>
#include <string_view>
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

	enum class EFileChangeType;
	class GlobalStorage;

	class AssetsManager : public IAssetsManager, private ILogProvider
	{
	  protected:
		Context &_context;
		std::vector<PackInfo> _founded;
		std::vector<PackInfo> _loaded;
		void *_developerDirectoryWatch;
		DelegateEvent<const std::filesystem::path &, EFileChangeType> _developerFilesTrigger;

	  public:
		explicit AssetsManager(Context &context) noexcept;
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
		void DeveloperDirectoryWatchCallback(const std::filesystem::path &filePath, EFileChangeType type);
		std::vector<std::vector<std::byte>> CollectPackageFileBytes() noexcept;
	};
} // namespace tudov
