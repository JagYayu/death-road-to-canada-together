#include "data/GlobalStorage.hpp"

#include "data/PathInfo.hpp"
#include "data/StorageEnumerationResult.hpp"

#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_storage.h"
#include "SDL3/SDL_timer.h"

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
	EStorageEnumerationResult result;
};

SDL_EnumerationResult StorageEnumerateDirectoryCallback(void *userdata, const char *dirname, const char *fname)
{
	auto storageUserdata = static_cast<StorageEnumerateDirectoryCallbackUserdata *>(userdata);
	std::string_view directory{dirname};
	std::string_view file{fname};
	storageUserdata->result = storageUserdata->callback(file, directory, storageUserdata->callbackArgs);
	return static_cast<SDL_EnumerationResult>(storageUserdata->result);
}

EStorageEnumerationResult GlobalStorage::EnumerateDirectory(std::string_view path, const IStorage::EnumerationCallbackFunction<> &callback, void *callbackArgs) noexcept
{
	for (std::size_t i = 0; i < 200; ++i) // TODO
	{
		if (IsReady())
		{
			break;
		}
		SDL_Delay(1);
	}

	StorageEnumerateDirectoryCallbackUserdata userdata{
	    .callback = callback,
	    .callbackArgs = callbackArgs,
	    .result = EStorageEnumerationResult::Continue,
	};
	SDL_EnumerateStorageDirectory(_sdlStorage, path.data(), StorageEnumerateDirectoryCallback, &userdata);
	return userdata.result;
}

bool GlobalStorage::Exists(std::string_view path) noexcept
{
	SDL_PathInfo info;
	SDL_GetStoragePathInfo(_sdlStorage, path.data(), &info);
	return info.type != SDL_PATHTYPE_NONE;
}

// PathInfo GlobalStorage::GetPathInfo(std::string_view path) noexcept
// {
// 	SDL_PathInfo info;
// 	SDL_GetStoragePathInfo(_storage, path.data(), &info);
// 	return std::bit_cast<PathInfo>(info);
// }

std::uint64_t GlobalStorage::GetPathSize(std::string_view filePath) noexcept
{
	// TODO query directory path size?
	std::uint64_t size = 0;
	SDL_GetStorageFileSize(_sdlStorage, filePath.data(), &size);
	return size;
}

EPathType GlobalStorage::GetPathType(std::string_view path) noexcept
{
	SDL_PathInfo info;
	SDL_GetStoragePathInfo(_sdlStorage, path.data(), &info);
	return static_cast<EPathType>(info.type);
}

std::vector<std::byte> GlobalStorage::ReadFileToBytes(std::string_view filePath)
{
	if (CanRead())
	{
		throw std::runtime_error("can't read");
	}

	auto size = GetPathSize(filePath);
	void *dst = SDL_malloc(size);
	bool success = SDL_ReadStorageFile(_sdlStorage, "save0.sav", dst, size);
	SDL_free(dst);

	if (success)
	{
		auto bytePtr = static_cast<const std::byte *>(dst);
		return std::vector<std::byte>(bytePtr, bytePtr + size / sizeof(std::string::value_type));
		// return (str, size / sizeof(std::string::value_type));
	}
	else
	{
		auto err = SDL_GetError();
		throw std::runtime_error(err);
	}
}

// std::string GlobalStorage::ReadFileToString(std::string_view filePath)
// {
// 	throw std::runtime_error("DO NOT USE!");
// 	// return ReadFileToBytes(filePath);
// }
