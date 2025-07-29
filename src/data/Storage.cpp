#include "data/Storage.hpp"

#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_storage.h"

using namespace tudov;

enum class IStorage::EEnumerationResult
{
	Continue = SDL_EnumerationResult::SDL_ENUM_CONTINUE,
	Success = SDL_EnumerationResult::SDL_ENUM_SUCCESS,
	Failure = SDL_EnumerationResult::SDL_ENUM_FAILURE,
};

Storage::Storage() noexcept
    : _storage(nullptr)
{
}

Storage::~Storage() noexcept
{
	if (_storage != nullptr)
	{
		if (!SDL_CloseStorage(_storage))
		{
			Error("Error while close storage", SDL_GetError());
		}

		_storage = nullptr;
	}
}

bool Storage::IsReady() noexcept
{
	return SDL_StorageReady(_storage);
}

bool Storage::CanRead() noexcept
{
	return true;
}

struct StorageEnumerateDirectoryCallbackUserdata
{
	const IStorage::EnumerationCallbackFunction<> &callback;
	void *callbackArgs;
};

SDL_EnumerationResult StorageEnumerateDirectoryCallback(void *userdata, const char *dirname, const char *fname)
{
	auto storageUserdata = static_cast<StorageEnumerateDirectoryCallbackUserdata *>(userdata);
	std::string_view directory{dirname};
	std::string_view file{fname};
	IStorage::EEnumerationResult result = storageUserdata->callback(storageUserdata->callbackArgs, directory, file);
	return static_cast<SDL_EnumerationResult>(result);
}

void Storage::EnumerateDirectory(std::string_view path, const std::function<EEnumerationResult(void *, std::string_view, std::string_view)> &callback, void *callbackArgs) noexcept
{
	StorageEnumerateDirectoryCallbackUserdata userdata{
	    .callback = callback,
	    .callbackArgs = callbackArgs,
	};
	SDL_EnumerateStorageDirectory(_storage, path.data(), StorageEnumerateDirectoryCallback, &userdata);
}
