/**
 * @file data/AssetsManager.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "data/AssetsManager.hpp"

#include "data/Config.hpp"
#include "data/Constants.hpp"
#include "data/GlobalStorage.hpp"
#include "data/GlobalStorageLocation.hpp"
#include "data/GlobalStorageManager.hpp"
#include "data/HierarchyIterationResult.hpp"
#include "data/VirtualFileSystem.hpp"
#include "data/ZipStorage.hpp"
#include "program/Engine.hpp"
#include "resource/GlobalResourcesCollection.hpp"
#include "util/FileChangeType.hpp"

#include "FileWatch.hpp"
#include "util/LogMicros.hpp"
#include "util/StringUtils.hpp"

#include <filesystem>
#include <memory>
#include <string_view>
#include <tuple>
#include <vector>

#undef UpdateResource

using namespace tudov;

using DeveloperDirectoryWatch = filewatch::FileWatch<std::string>;

AssetsManager::AssetsManager(Context &context) noexcept
    : _context(context),
      _developerDirectoryWatch(nullptr)
{
}

AssetsManager::~AssetsManager() noexcept
{
	if (_developerDirectoryWatch != nullptr)
	{
		delete static_cast<DeveloperDirectoryWatch *>(_developerDirectoryWatch);
		_developerDirectoryWatch = nullptr;
	}
}

Context &AssetsManager::GetContext() noexcept
{
	return _context;
}

Log &AssetsManager::GetLog() noexcept
{
	return *Log::Get("AssetsManager");
}

DelegateEvent<const std::filesystem::path &, EFileChangeType> &AssetsManager::GetOnDeveloperFilesTrigger() noexcept
{
	return _developerFilesTrigger;
}

void AssetsManager::Initialize() noexcept
{
	LoadAssetsFromPackageFiles();
	LoadAssetsFromDeveloperDirectory();
	LoadAssetsFromExternalDirectories();
}

void AssetsManager::LoadAssetsFromPackageFiles() noexcept
{
	for (auto &&bytes : CollectPackageFileBytes())
	{
		ZipStorage zipStorage{bytes};

		zipStorage.Foreach("", [this](const std::filesystem::path &filePath, const std::filesystem::path &, void *) -> EHierarchyIterationResult
		{
			Info("Loading contents from package: {}", filePath.generic_string());
			// TODO

			return EHierarchyIterationResult::Continue;
		});
	}
}

std::vector<std::vector<std::byte>> AssetsManager::CollectPackageFileBytes() noexcept
{
	std::vector<std::vector<std::byte>> zipFileBytes;

	GlobalStorage &applicationGlobalStorage = GetGlobalStorageManager().GetApplicationStorage();

	applicationGlobalStorage.Foreach("", [this, &applicationGlobalStorage, &zipFileBytes](const std::filesystem::path &filePath, const std::filesystem::path &directoryPath, void *) -> EHierarchyIterationResult
	{
		if (std::filesystem::is_regular_file(filePath))
		{
			if (std::filesystem::path(filePath).extension() == ".pck")
			{
				zipFileBytes.emplace_back(applicationGlobalStorage.ReadFileToBytes(filePath));
			}
		}

		return EHierarchyIterationResult::Continue;
	});

	return zipFileBytes;
}

void AssetsManager::LoadAssetsFromDeveloperDirectory() noexcept
{
	TE_DEBUG("{}", "Loading assets from developer directory");

	GlobalStorage &applicationGlobalStorage = GetGlobalStorageManager().GetApplicationStorage();
	if (!applicationGlobalStorage.CanRead())
	{
		return;
	}

	std::vector<std::tuple<std::string, std::vector<std::byte>>> assets{};

	applicationGlobalStorage.ForeachRecursed(Constants::DataDeveloperAssetsDirectory, [this, &assets, &applicationGlobalStorage](const std::filesystem::path &path, const std::filesystem::path &directory, void *) -> EHierarchyIterationResult
	{
		if (path.has_extension())
		{
			std::filesystem::path fileFillPath = directory / path;
			std::filesystem::path resourcePath = Constants::DataVirtualStorageRootApp / std::filesystem::relative(fileFillPath, Constants::DataDeveloperAssetsDirectory);
			auto &&bytes = applicationGlobalStorage.ReadFileToBytes(fileFillPath);

			auto path = resourcePath.generic_string();
			TE_TRACE("\"{}\", {} bytes", path, bytes.size());
			assets.emplace_back(path, bytes);
		}

		return EHierarchyIterationResult::Continue;
	});

	for (auto &&[path, bytes] : assets)
	{
		GetVirtualFileSystem().MountFile(path, bytes);
	}

	if (GlobalStorageLocation::IsAccessible())
	{
		try
		{
			std::filesystem::path absolutePath = GlobalStorageLocation::GetPath(EGlobalStorageLocation::Application) / Constants::DataDeveloperAssetsDirectory;
			_developerDirectoryWatch = new DeveloperDirectoryWatch(absolutePath.generic_string(), [this](std::string_view path, const filewatch::Event changeType)
			{
				auto filePath = Constants::DataDeveloperAssetsDirectory / std::filesystem::path(path);
				DeveloperDirectoryWatchCallback(filePath, static_cast<EFileChangeType>(changeType));
			});
		}
		catch (std::exception &e)
		{
			Error("{}", e.what());
		}
	}
}

void AssetsManager::DeveloperDirectoryWatchCallback(const std::filesystem::path &filePath, EFileChangeType type)
{
	auto fileExtension = filePath.extension();
	if (fileExtension.has_extension())
	{
		std::string file = filePath.generic_string();
		Debug("File in developer directory changed, file: \"{}\", type: {}", file, static_cast<int>(type));

		_developerFilesTrigger.Invoke(filePath, std::move(type));

		GlobalStorage &applicationGlobalStorage = GetGlobalStorageManager().GetApplicationStorage();
		GlobalResourcesCollection &globalResourcesCollection = GetGlobalResourcesCollection();
		std::vector<std::byte> bytes = applicationGlobalStorage.ReadFileToBytes(file);

		switch (type)
		{
		case EFileChangeType::Added:
		case EFileChangeType::Removed:
		case EFileChangeType::Modified:
			globalResourcesCollection.ReloadResource(filePath, bytes);
		case EFileChangeType::RenamedOld:
		case EFileChangeType::RenamedNew:
			break;
		}
	}
}

void AssetsManager::LoadAssetsFromExternalDirectories() noexcept
{
	TE_DEBUG("{}", "Loading assets from external directories...");

	const Config &config = GetConfig();

	std::vector<std::string> externalDirectories = config.GetMountDirectories();

	if (CanTrace())
	{
		for (const std::string &directory : externalDirectories)
		{
			Trace("{}", directory);
		}
	}

	auto &&renderBackend = config.GetRenderBackend();

	// auto &&loadTexture = [this, renderBackend]() {
	// 	switch (renderBackend) {
	// 		textureManager.Load<>(file, window.renderer, std::string_view(data))
	// 	}
	// };

	std::unordered_map<EResourceType, std::uint32_t> fileCounts{};

	std::vector<std::regex> mountBitmapPatterns{};
	for (auto &&pattern : config.GetMountBitmaps())
	{
		mountBitmapPatterns.emplace_back(std::regex(std::string(pattern), std::regex_constants::icase));
	}

	auto &&mountDirectories = config.GetMountFiles();
	for (auto &&mountDirectory : externalDirectories)
	{
		if (!std::filesystem::exists(mountDirectory.data()) || !std::filesystem::is_directory(mountDirectory.data()))
		{
			TE_WARN("Invalid directory for external assets: {}", mountDirectory.data());

			continue;
		}

		for (auto &entry : std::filesystem::recursive_directory_iterator(mountDirectory))
		{
			if (!entry.is_regular_file())
			{
				continue;
			}

			auto &&path = entry.path();
			auto &&it = mountDirectories.find(path.extension().generic_string());
			if (it == mountDirectories.end())
			{
				continue;
			}

			GetVirtualFileSystem().MountFile(path);
		}
	}

	TE_DEBUG("{}", "Loaded assets from external directories");
	if (CanTrace())
	{
		for (auto [fileType, count] : fileCounts)
		{
			Trace("{}: {}", ResourceType::ToStringView(fileType).data(), count);
		}
	}
}

void AssetsManager::Deinitialize() noexcept
{
	//
}

const std::vector<IAssetsManager::PackInfo> &AssetsManager::GetFounded() const noexcept
{
	return _founded;
}

const std::vector<IAssetsManager::PackInfo> &AssetsManager::GetLoaded() const noexcept
{
	return _loaded;
}
