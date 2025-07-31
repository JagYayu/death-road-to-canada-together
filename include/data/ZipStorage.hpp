#pragma once

#include "Storage.hpp"

#include <span>
#include <string_view>
#include <vector>

namespace tudov
{
	enum class EPathType;
	
	class ZipStorage : public IStorage
	{
	  protected:
		std::vector<std::byte> _bytes;
		void *_unzip;

	  public:
		explicit ZipStorage(std::span<std::byte> data);
		~ZipStorage() noexcept override;

		virtual bool CanRead() noexcept override;

		virtual bool CanWrite() noexcept override;

		virtual bool IsReady() noexcept override;

		EStorageEnumerationResult EnumerateDirectory(std::string_view directory, const EnumerationCallbackFunction<> &callback, void *callbackArgs = nullptr) noexcept override;

		virtual bool Exists(std::string_view path) noexcept override;

		// virtual PathInfo GetPathInfo(std::string_view path) noexcept override;

		virtual std::uint64_t GetPathSize(std::string_view filePath) noexcept override;
		virtual EPathType GetPathType(std::string_view filePath) noexcept override;

		virtual std::vector<std::byte> ReadFileToBytes(std::string_view filePath) override;
		// virtual std::string ReadFileToString(std::string_view filePath) override;

		// virtual std::uint64_t GetFileCompressedSize(std::string_view filePath) noexcept;
	};
} // namespace tudov
