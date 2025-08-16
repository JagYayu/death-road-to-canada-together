/**
 * @file data/GlobalStorage.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "data/GlobalStorage.hpp"

#include "data/Hierarchy.hpp"
#include "data/HierarchyIterationResult.hpp"

#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_storage.h"
#include "data/PathType.hpp"

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
	EHierarchyIterationResult result;
};

SDL_EnumerationResult StorageEnumerateDirectoryCallback(void *userdata, const char *dirname, const char *fname)
{
	auto storageUserdata = static_cast<StorageEnumerateDirectoryCallbackUserdata *>(userdata);
	std::string_view directory{dirname};
	std::string_view file{fname};
	storageUserdata->result = storageUserdata->callback(file, directory, storageUserdata->callbackArgs);
	return static_cast<SDL_EnumerationResult>(storageUserdata->result);
}

EHierarchyIterationResult ForeachDirectoryImpl(SDL_Storage *_sdlStorage, const IHierarchy<PathData>::Path &path, const IStorage::EnumerationCallbackFunction<> &callback, void *callbackArgs)
{
	StorageEnumerateDirectoryCallbackUserdata userdata{
	    .callback = callback,
	    .callbackArgs = callbackArgs,
	    .result = EHierarchyIterationResult::Continue,
	};
	SDL_EnumerateStorageDirectory(_sdlStorage, path.generic_string().c_str(), StorageEnumerateDirectoryCallback, &userdata);
	return userdata.result;
}

EHierarchyIterationResult GlobalStorage::Foreach(const Path &path, const IStorage::EnumerationCallbackFunction<> &callback, void *callbackArgs) noexcept
{
	if (IsReady()) [[unlikely]]
	{
		return ForeachDirectoryImpl(_sdlStorage, path, callback, callbackArgs);
	}

	return EHierarchyIterationResult::Failure;
}

EHierarchyElement GlobalStorage::Check(const Path &path) noexcept
{
	SDL_PathInfo info;
	SDL_GetStoragePathInfo(_sdlStorage, path.generic_string().c_str(), &info);

	switch (info.type)
	{
	case SDL_PATHTYPE_FILE:
		return EHierarchyElement::Data;
	case SDL_PATHTYPE_DIRECTORY:
		return EHierarchyElement::Directory;
	default:
		return EHierarchyElement::None;
	};
}

PathInfo GlobalStorage::GetPathInfo(const Path &path) noexcept
{
	SDL_PathInfo info;
	SDL_GetStoragePathInfo(_sdlStorage, path.generic_string().data(), &info);

	EPathType type;
	switch (info.type)
	{
	case SDL_PATHTYPE_NONE:
		type = EPathType::None;
		break;
	case SDL_PATHTYPE_FILE:
		type = EPathType::File;
		break;
	case SDL_PATHTYPE_DIRECTORY:
		type = EPathType::Directory;
		break;
	case SDL_PATHTYPE_OTHER:
		type = EPathType::Other;
		break;
	}

	return PathInfo{
	    .type = type,
	    .size = info.size,
	    .createTimeNS = info.create_time,
	    .modifyTimeNS = info.modify_time,
	    .accessTimeNS = info.access_time,
	};
}

std::uint64_t GlobalStorage::GetPathSize(const Path &filePath) noexcept
{
	std::uint64_t size = 0;
	SDL_GetStorageFileSize(_sdlStorage, filePath.generic_string().c_str(), &size);
	return size;
}

EPathType GlobalStorage::GetPathType(const Path &path) noexcept
{
	SDL_PathInfo info;
	SDL_GetStoragePathInfo(_sdlStorage, path.generic_string().c_str(), &info);
	return static_cast<EPathType>(info.type);
}

std::vector<std::byte> GlobalStorage::ReadFileToBytes(const Path &filePath)
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
