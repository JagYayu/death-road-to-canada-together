#pragma once

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

		virtual ~IAssetsManager() noexcept = default;

		virtual const std::vector<PackInfo> &GetFounded() const noexcept = 0;
		virtual const std::vector<PackInfo> &GetLoaded() const noexcept = 0;
	};

	enum class EFileChangeType;
	class GlobalStorage;

	class AssetsManager : public IAssetsManager, public ILogProvider
	{
	  protected:
		Context &_context;
		std::vector<PackInfo> _founded;
		std::vector<PackInfo> _loaded;
		void *_developerDirectoryWatch;

	  public:
		explicit AssetsManager(Context &context) noexcept;
		~AssetsManager() noexcept override;

		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;
		void Initialize() noexcept override;
		void Deinitialize() noexcept override;

		const std::vector<PackInfo> &GetFounded() const noexcept override;
		const std::vector<PackInfo> &GetLoaded() const noexcept override;

	  private:
		void LoadAssetsFromPackageFiles() noexcept;
		void LoadAssetsFromDeveloperDirectory() noexcept;
		void DeveloperDirectoryWatchCallback(std::string_view filePath, EFileChangeType type);
		std::vector<std::vector<std::byte>> CollectPackageFileBytes() noexcept;
	};
} // namespace tudov
