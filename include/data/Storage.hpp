#pragma once

#include <filesystem>
#include <functional>
#include <string_view>
#include <vector>

struct SDL_Storage;

namespace tudov
{
	enum class EPathType;
	enum class EStorageIterationResult;
	struct IGlobalStorageManager;
	struct PathInfo;

	/**
	 * Interface Class
	 */
	struct IStorage
	{
		template <typename TCallbackArgs = void *>
		using EnumerationCallbackFunction = std::function<EStorageIterationResult(const std::filesystem::path &path, const std::filesystem::path &directory, TCallbackArgs args)>;

		virtual ~IStorage() noexcept = default;

		virtual bool CanRead() noexcept = 0;

		virtual bool CanWrite() noexcept = 0;

		virtual bool IsReady() noexcept = 0;

		virtual EStorageIterationResult ForeachDirectory(const std::filesystem::path &directory, const EnumerationCallbackFunction<> &callback, void *callbackArgs = nullptr) noexcept = 0;

		virtual EStorageIterationResult ForeachDirectoryRecursed(const std::filesystem::path &directory, const EnumerationCallbackFunction<> &callback, std::uint32_t maxDepth = -1, void *callbackArgs = nullptr) noexcept;

		virtual bool Exists(const std::filesystem::path &path) noexcept = 0;

		// virtual PathInfo GetPathInfo(std::string_view path) noexcept = 0;

		virtual std::uint64_t GetPathSize(const std::filesystem::path &filePath) noexcept = 0;
		virtual EPathType GetPathType(const std::filesystem::path &path) noexcept = 0;

		virtual std::vector<std::byte> ReadFileToBytes(const std::filesystem::path &filePath) = 0;
		// virtual std::string ReadFileToString(std::string_view filePath) = 0;

		template <typename TCallbackArgs>
		inline EStorageIterationResult ForeachDirectory(const std::filesystem::path &path, const EnumerationCallbackFunction<TCallbackArgs *> &callback, TCallbackArgs *callbackArgs = nullptr) noexcept
		{
			return ForeachDirectory(path, callback, callbackArgs);
		}

		// template <typename TCallbackArgs = void>
		// inline EStorageIterationResult ForeachDirectoryRecursed(const std::filesystem::path &path, const EnumerationCallbackFunction<> &callback, TCallbackArgs *callbackArgs = nullptr) noexcept
		// {
		// }

		// using Iteration = CommonIteration<EStorageIterationResult, std::string_view>;
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
