#pragma once

#include "util/CommonIteration.hpp"

#include <functional>
#include <string_view>
#include <vector>

struct SDL_Storage;

namespace tudov
{
	enum class EPathType;
	enum class EStorageEnumerationResult;
	struct IGlobalStorageManager;
	struct PathInfo;

	/**
	 * Interface Class
	 */
	struct IStorage
	{
		template <typename TCallbackArgs = void *>
		using EnumerationCallbackFunction = std::function<EStorageEnumerationResult(std::string_view file, std::string_view directory, TCallbackArgs args)>;

		virtual ~IStorage() noexcept = default;

		virtual bool CanRead() noexcept = 0;

		virtual bool CanWrite() noexcept = 0;

		virtual bool IsReady() noexcept = 0;

		virtual EStorageEnumerationResult EnumerateDirectory(std::string_view directoryPath, const EnumerationCallbackFunction<> &callback, void *callbackArgs = nullptr) noexcept = 0;

		virtual bool Exists(std::string_view path) noexcept = 0;

		// virtual PathInfo GetPathInfo(std::string_view path) noexcept = 0;

		virtual std::uint64_t GetPathSize(std::string_view filePath) noexcept = 0;
		virtual EPathType GetPathType(std::string_view path) noexcept = 0;

		virtual std::vector<std::byte> ReadFileToBytes(std::string_view filePath) = 0;
		// virtual std::string ReadFileToString(std::string_view filePath) = 0;

		template <typename TCallbackArgs>
		inline EStorageEnumerationResult EnumerateDirectory(std::string_view path, const EnumerationCallbackFunction<TCallbackArgs *> &callback, TCallbackArgs *callbackArgs = nullptr) noexcept
		{
			return EnumerateDirectory(path, callback, callbackArgs);
		}

		// using Iteration = CommonIteration<EStorageEnumerationResult, std::string_view>;
		// Iteration iteration;
	};

	// class Storage : public virtual IStorage, public ILogProvider
	// {
	//   protected:
	// 	SDL_Storage *_storage;

	//   public:
	// 	explicit Storage() noexcept;
	// 	virtual ~Storage() noexcept;

	// 	bool IsReady() noexcept override;

	// 	bool CanRead() noexcept override;

	// 	void EnumerateDirectory(std::string_view path, const EnumerationCallbackFunction<> &callback, void *callbackArgs) noexcept override;

	// 	bool Exists(std::string_view path) noexcept override;

	// 	// PathInfo GetPathInfo(std::string_view path) noexcept override;

	// 	std::uint64_t GetFileSize(std::string_view filePath) noexcept override;

	// 	std::vector<std::byte> ReadFileToBytes(std::string_view filePath) override;
	// 	std::string ReadFileToString(std::string_view filePath) override;
	// };
} // namespace tudov
