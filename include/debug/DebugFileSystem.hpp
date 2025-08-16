/**
 * @file debug/DebugFileSystem.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "DebugElement.hpp"

#include <array>
#include <filesystem>

namespace tudov
{
	class DebugFileSystem : public IDebugElement
	{
	  public:
		inline static constexpr std::string_view Name() noexcept
		{
			return "File System";
		}

	  protected:
		struct FileEntry
		{
			std::string name;
			bool isDirectory;
			std::string dateModified;
			std::string type;
			std::double_t sizeKB;
		};

		std::array<char, 256> _browsePath;
		std::filesystem::path _currentPath;
		std::string _selectedFile;
		std::vector<FileEntry> _entries;
		bool _refresh;

	  public:
		explicit DebugFileSystem() noexcept;
		~DebugFileSystem() noexcept override;

	  public:
		std::string_view GetName() noexcept override;
		void UpdateAndRender(IWindow &window) noexcept override;
		void OnOpened(IWindow &window) noexcept override;
		void OnClosed(IWindow &window) noexcept override;

	  protected:
		void Refresh(VirtualFileSystem &vfs) noexcept;
	};
} // namespace tudov
