/**
 * @file data/Storage.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Hierarchy.hpp"
#include "HierarchyElement.hpp"
#include "PathData.hpp"

#include <filesystem>
#include <functional>
#include <vector>

struct SDL_Storage;

namespace tudov
{
	enum class EPathType;
	enum class EHierarchyIterationResult;
	struct IGlobalStorageManager;
	struct PathInfo;

	/**
	 * Interface Class
	 */
	struct IStorage : public IHierarchy<PathData>
	{
		template <typename TCallbackArgs = void *>
		using EnumerationCallbackFunction = std::function<EHierarchyIterationResult(const std::filesystem::path &path, const std::filesystem::path &directory, TCallbackArgs args)>;

		virtual ~IStorage() noexcept = default;

		virtual bool CanRead() noexcept = 0;

		virtual bool CanWrite() noexcept = 0;

		virtual bool IsReady() noexcept = 0;

		virtual PathInfo GetPathInfo(const std::filesystem::path &path) noexcept = 0;

		virtual std::uint64_t GetPathSize(const std::filesystem::path &filePath) noexcept = 0;

		virtual EPathType GetPathType(const std::filesystem::path &path) noexcept = 0;

		virtual std::vector<std::byte> ReadFileToBytes(const std::filesystem::path &filePath) = 0;

		inline bool Exists(const std::filesystem::path &path) noexcept
		{
			return Check(path) != EHierarchyElement::None;
		}

		inline PathData Get(const Path &dataPath) override
		{
			return PathData{
			    .info = GetPathInfo(dataPath),
			    .byte = ReadFileToBytes(dataPath),
			};
		}

		template <typename TCallbackArgs>
		inline EHierarchyIterationResult ForeachDirectory(const std::filesystem::path &path, const EnumerationCallbackFunction<TCallbackArgs *> &callback, TCallbackArgs *callbackArgs = nullptr) noexcept
		{
			return ForeachDirectory(path, callback, callbackArgs);
		}
	};
} // namespace tudov
