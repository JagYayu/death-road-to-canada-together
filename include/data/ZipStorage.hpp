/**
 * @file data/ZipStorage.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "HierarchyElement.hpp"
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

		EHierarchyIterationResult Foreach(const std::filesystem::path &directory, const EnumerationCallbackFunction<> &callback, void *callbackArgs = nullptr) noexcept override;

		virtual EHierarchyElement Check(const std::filesystem::path &path) noexcept override;

		virtual PathInfo GetPathInfo(const std::filesystem::path& path) noexcept override;

		virtual std::uint64_t GetPathSize(const std::filesystem::path &filePath) noexcept override;

		virtual EPathType GetPathType(const std::filesystem::path &filePath) noexcept override;

		virtual std::vector<std::byte> ReadFileToBytes(const std::filesystem::path &filePath) override;
		// virtual std::string ReadFileToString(const std::filesystem::path& filePath) override;

		// virtual std::uint64_t GetFileCompressedSize(const std::filesystem::path& filePath) noexcept;
	};
} // namespace tudov
