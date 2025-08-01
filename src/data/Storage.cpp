#include "data/Storage.hpp"

#include "data/StorageIterationResult.hpp"

#include <filesystem>

using namespace tudov;

EStorageIterationResult IStorage::ForeachDirectoryRecursed(const std::filesystem::path &directory, const EnumerationCallbackFunction<> &callback, std::uint32_t maxDepth, void *callbackArgs) noexcept
{
	if (maxDepth == 0)
	{
		return EStorageIterationResult::Success;
	}

	auto &&callbackRecursed = [this, &callback, maxDepth](const std::filesystem::path &path, const std::filesystem::path &directory, void *args) -> EStorageIterationResult
	{
		if (!path.has_extension())
		{
			EStorageIterationResult result = ForeachDirectoryRecursed(directory / path, callback, maxDepth - 1, args);
			switch (result)
			{
			case EStorageIterationResult::Continue:
				break;
			default:
				return result;
			}
		}

		return callback(path, directory, args);
	};

	return ForeachDirectory(directory, callbackRecursed, callbackArgs);
}
