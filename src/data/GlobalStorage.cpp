#include "data/GlobalStorage.hpp"

#include "data/PathInfo.hpp"
#include "data/StorageIterationResult.hpp"

#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_storage.h"
#include "SDL3/SDL_timer.h"

#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <string_view>

using namespace tudov;

GlobalStorage::GlobalStorage(IGlobalStorageManager &globalStorageManager) noexcept
    : _globalStorageManager(globalStorageManager),
      _sdlStorage(nullptr)
{
}

GlobalStorage::~GlobalStorage() noexcept
{
	if (_sdlStorage != nullptr)
	{
		if (!SDL_CloseStorage(_sdlStorage))
		{
			Error("Error while closing global storage", SDL_GetError());
		}

		_sdlStorage = nullptr;
	}
}

bool GlobalStorage::IsReady() noexcept
{
	return SDL_StorageReady(_sdlStorage);
}

bool GlobalStorage::CanRead() noexcept
{
	return true;
}

struct StorageEnumerateDirectoryCallbackUserdata
{
	const IStorage::EnumerationCallbackFunction<> &callback;
	void *callbackArgs;
	EStorageIterationResult result;
};

SDL_EnumerationResult StorageEnumerateDirectoryCallback(void *userdata, const char *dirname, const char *fname)
{
	auto storageUserdata = static_cast<StorageEnumerateDirectoryCallbackUserdata *>(userdata);
	std::string_view directory{dirname};
	std::string_view file{fname};
	storageUserdata->result = storageUserdata->callback(file, directory, storageUserdata->callbackArgs);
	return static_cast<SDL_EnumerationResult>(storageUserdata->result);
}

EStorageIterationResult ForeachDirectoryImpl(SDL_Storage *_sdlStorage, const std::filesystem::path &path, const IStorage::EnumerationCallbackFunction<> &callback, void *callbackArgs)
{
	StorageEnumerateDirectoryCallbackUserdata userdata{
	    .callback = callback,
	    .callbackArgs = callbackArgs,
	    .result = EStorageIterationResult::Continue,
	};
	SDL_EnumerateStorageDirectory(_sdlStorage, path.generic_string().c_str(), StorageEnumerateDirectoryCallback, &userdata);
	return userdata.result;
}

EStorageIterationResult GlobalStorage::ForeachDirectory(const std::filesystem::path &path, const IStorage::EnumerationCallbackFunction<> &callback, void *callbackArgs) noexcept
{
	if (IsReady()) [[unlikely]]
	{
		return ForeachDirectoryImpl(_sdlStorage, path, callback, callbackArgs);
	}

	return EStorageIterationResult::Failure;
}

bool GlobalStorage::Exists(const std::filesystem::path &path) noexcept
{
	SDL_PathInfo info;
	SDL_GetStoragePathInfo(_sdlStorage, path.generic_string().c_str(), &info);
	return info.type != SDL_PATHTYPE_NONE;
}

// PathInfo GlobalStorage::GetPathInfo(const std::filesystem::path &path) noexcept
// {
// 	SDL_PathInfo info;
// 	SDL_GetStoragePathInfo(_storage, path.data(), &info);
// 	return std::bit_cast<PathInfo>(info);
// }

std::uint64_t GlobalStorage::GetPathSize(const std::filesystem::path &filePath) noexcept
{
	std::uint64_t size = 0;
	SDL_GetStorageFileSize(_sdlStorage, filePath.generic_string().c_str(), &size);
	return size;
}

EPathType GlobalStorage::GetPathType(const std::filesystem::path &path) noexcept
{
	SDL_PathInfo info;
	SDL_GetStoragePathInfo(_sdlStorage, path.generic_string().c_str(), &info);
	return static_cast<EPathType>(info.type);
}

std::vector<std::byte> GlobalStorage::ReadFileToBytes(const std::filesystem::path &filePath)
{
	if (!CanRead()) [[unlikely]]
	{
		throw std::runtime_error("can't read");
	}

	std::size_t size = GetPathSize(filePath);
	if (size == 0) [[unlikely]]
	{
		return {};
	}

	void *dst = SDL_malloc(size);
	bool success = SDL_ReadStorageFile(_sdlStorage, filePath.generic_string().data(), dst, size);

	if (!success)
	{
		SDL_free(dst);
		auto err = SDL_GetError();
		throw std::runtime_error(err);
	}

	auto bytePtr = static_cast<const std::byte *>(dst);
	auto bytes = std::vector<std::byte>(bytePtr, bytePtr + size);
	SDL_free(dst);
	return bytes;
}

// std::string GlobalStorage::ReadFileToString(std::string_view filePath)
// {
// 	throw std::runtime_error("DO NOT USE!");
// 	// return ReadFileToBytes(filePath);
// }
