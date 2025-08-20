/**
 * @file util/FileSystemWatch.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Micros.hpp"
#include "data/AssetsManager.hpp"
#include "event/DelegateEvent.hpp"

#include <filesystem>

namespace tudov
{
	enum class EFileChangeType : int;
	enum class EPathType : int;

	class FileSystemWatch
	{
	  private:
		void *_fileWatch;
		std::filesystem::path _path;
		std::atomic<bool> _watching;
		DelegateEvent<std::string_view, EPathType, EFileChangeType> _onCallback;

	  public:
		explicit FileSystemWatch() noexcept;
		explicit FileSystemWatch(const std::filesystem::path &path) noexcept;
		~FileSystemWatch() noexcept;

		decltype(_onCallback) &GetOnCallback() noexcept;
		std::filesystem::path GetPath() const noexcept;
		void SetPath(const std::filesystem::path &path) noexcept;
		bool IsWatching() const noexcept;
		void StartWatching();
		void StopWatching();

		TE_FORCEINLINE const decltype(_onCallback) &GetOnCallback() const noexcept
		{
			return const_cast<FileSystemWatch *>(this)->GetOnCallback();
		}
	};
} // namespace tudov
