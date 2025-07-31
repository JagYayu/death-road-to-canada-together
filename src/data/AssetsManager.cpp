#include "data/AssetsManager.hpp"

#include "FileWatch.hpp"
#include "data/GlobalStorage.hpp"
#include "data/GlobalStorageLocation.hpp"
#include "data/GlobalStorageManager.hpp"
#include "data/StorageEnumerationResult.hpp"
#include "data/ZipStorage.hpp"
#include "program/Engine.hpp"
#include "util/FileChangeType.hpp"

#include <filesystem>
#include <memory>
#include <string_view>
#include <vector>

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

void AssetsManager::Initialize() noexcept
{
	LoadAssetsFromPackageFiles();
#ifdef WIN32
	LoadAssetsFromDeveloperDirectory();
#endif
}

void AssetsManager::LoadAssetsFromPackageFiles() noexcept
{
	for (auto &&bytes : CollectPackageFileBytes())
	{
		ZipStorage zipStorage{bytes};

		zipStorage.EnumerateDirectory("", [this](std::string_view filePath, std::string_view, void *) -> EStorageEnumerationResult
		{
			Info("Loading contents from package: {}", filePath);
			// TODO

			return EStorageEnumerationResult::Continue;
		});
	}
}

std::vector<std::vector<std::byte>> AssetsManager::CollectPackageFileBytes() noexcept
{
	std::vector<std::vector<std::byte>> zipFileBytes;

	GlobalStorage &applicationGlobalStorage = GetGlobalStorageManager().GetApplicationStorage();

	applicationGlobalStorage.EnumerateDirectory("", [this, &applicationGlobalStorage, &zipFileBytes](std::string_view filePath, std::string_view directoryPath, void *) -> EStorageEnumerationResult
	{
		if (std::filesystem::is_regular_file(filePath))
		{
			if (std::filesystem::path(filePath).extension() == ".pck")
			{
				zipFileBytes.emplace_back(applicationGlobalStorage.ReadFileToBytes(filePath));
			}
		}

		Info("dir: {}, path: {}", directoryPath, filePath);
		return EStorageEnumerationResult::Continue;
	});

	return zipFileBytes;
}

void AssetsManager::LoadAssetsFromDeveloperDirectory() noexcept
{
	GlobalStorage &applicationGlobalStorage = GetGlobalStorageManager().GetApplicationStorage();

	try
	{
		auto path = GlobalStorageLocation::GetPath(EGlobalStorageLocation::Application) / Constants::AppDeveloperAssetsDirectory;
		_developerDirectoryWatch = new DeveloperDirectoryWatch(path.string(), [this](std::string_view path, const filewatch::Event changeType)
		{
			DeveloperDirectoryWatchCallback(path, static_cast<EFileChangeType>(changeType));
		});

		applicationGlobalStorage.EnumerateDirectory("", [this](std::string_view filePath, std::string_view, void *) -> EStorageEnumerationResult
		{
			Info("Loading contents from package: {}", filePath);
			// TODO

			return EStorageEnumerationResult::Continue;
		});
	}
	catch (std::exception &e)
	{
		Error("{}", e.what());
	}
}

void AssetsManager::DeveloperDirectoryWatchCallback(std::string_view filePath, EFileChangeType type)
{
	std::filesystem::path pFile = filePath;
	auto fileExtension = pFile.extension();

	switch (type)
	{
	case EFileChangeType::Added:
	case EFileChangeType::Removed:
	case EFileChangeType::Modified:
	case EFileChangeType::RenamedOld:
	case EFileChangeType::RenamedNew:
		break;
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
