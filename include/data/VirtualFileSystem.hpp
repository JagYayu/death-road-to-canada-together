/**
 * @file data/VirtualFileSystem.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "PathListOption.hpp"
#include "event/DelegateEvent.hpp"
#include "program/Context.hpp"
#include "resource/ResourceType.hpp"
#include "system/Log.hpp"

#include "sol/table.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <map>
#include <span>
#include <string>
#include <variant>
#include <vector>

namespace tudov
{
	enum class EPathType;
	class LuaBindings;

	class VirtualFileSystem : public IContextProvider, private ILogProvider
	{
		friend LuaBindings;

	  public:
		using MountDirectoryEvent = DelegateEvent<const std::filesystem::path &>;
		using DismountDirectoryEvent = MountDirectoryEvent;
		using MountFileEvent = DelegateEvent<const std::filesystem::path &, const std::vector<std::byte> &, EResourceType &>;
		using DismountFileEvent = MountDirectoryEvent;
		using RemountFileEvent = DelegateEvent<const std::filesystem::path &, const std::vector<std::byte> &, const std::vector<std::byte> &, EResourceType &>;

		struct ListEntry
		{
			std::string path;
			bool isDirectory;

			bool operator<(const ListEntry &r) const noexcept;
		};

	  protected:
		struct DirectoryNode;
		struct FileNode;
		using Node = std::variant<DirectoryNode, FileNode>;

		struct CommonNode
		{
			std::chrono::time_point<std::chrono::system_clock> date;

			explicit CommonNode() noexcept;
		};

		struct DirectoryNode : public CommonNode
		{
			std::map<std::string, Node> children;
			mutable bool cached;
			mutable size_t cachedSize;

			explicit DirectoryNode(const std::map<std::string, Node> &children = {}) noexcept;

			size_t GetSize() const noexcept;
		};

		struct FileNode : public CommonNode
		{
			std::vector<std::byte> bytes;
			EResourceType resourceType;

			explicit FileNode(const std::vector<std::byte> &bytes = {}, EResourceType resourceType = EResourceType::Unknown) noexcept;
		};

	  protected:
		Context &_context;
		Node _rootNode;
		MountDirectoryEvent _onMountDirectory;
		DismountDirectoryEvent _onDismountDirectory;
		MountFileEvent _onMountFile;
		DismountFileEvent _onDismountFile;
		RemountFileEvent _onRemountFile;

	  public:
		explicit VirtualFileSystem(Context &context) noexcept;
		explicit VirtualFileSystem(const VirtualFileSystem &) noexcept = default;
		explicit VirtualFileSystem(VirtualFileSystem &&) noexcept = default;
		VirtualFileSystem &operator=(const VirtualFileSystem &) noexcept = delete;
		VirtualFileSystem &operator=(VirtualFileSystem &&) noexcept = delete;
		~VirtualFileSystem() noexcept override = default;

		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		MountDirectoryEvent &GetOnMountDirectory() noexcept;
		DismountDirectoryEvent &GetOnDismountDirectory() noexcept;
		MountFileEvent &GetOnMountFile() noexcept;
		DismountFileEvent &GetOnDismountFile() noexcept;
		RemountFileEvent &GetOnRemountFile() noexcept;

		void MountDirectory(const std::filesystem::path &path) noexcept;
		void MountFile(const std::filesystem::path &path) noexcept;
		void MountFile(const std::filesystem::path &path, std::span<const std::byte> bytes) noexcept;
		bool DismountDirectory(const std::filesystem::path &path) noexcept;
		bool DismountFile(const std::filesystem::path &path) noexcept;
		bool RemountFile(const std::filesystem::path &path, const std::vector<std::byte> &bytes) noexcept;

		bool Exists(const std::filesystem::path &path) noexcept;
		EPathType GetPathType(const std::filesystem::path &path) noexcept;
		std::chrono::time_point<std::chrono::system_clock> GetPathDateModified(const std::filesystem::path &path);
		std::span<std::byte> GetFileBytes(const std::filesystem::path &file);
		EResourceType GetFileResourceType(const std::filesystem::path &file);
		std::size_t GetPathSize(const std::filesystem::path &file) const;

		std::vector<ListEntry> List(const std::filesystem::path &directory, EPathListOption options = EPathListOption::Default) const;

	  protected:
		Node *FindNode(const std::filesystem::path &path) noexcept;
		void CollectListEntries(std::vector<ListEntry> &entries, const std::filesystem::path &directory, const DirectoryNode *directoryNode, EPathListOption options, std::uint32_t depth) const noexcept;
		void ClearParentDirectoriesCache(const std::filesystem::path &path) const;

	  private:
		bool LuaExists(sol::object path);
		sol::table LuaList(sol::object directory, sol::object options);
		std::string LuaReadFile(sol::object file);

	  public:
		inline std::span<const std::byte> GetFileBytes(const std::filesystem::path &path) const
		{
			return const_cast<VirtualFileSystem *>(this)->GetFileBytes(path);
		}

	  protected:
		inline const Node *FindNode(const std::filesystem::path &path) const noexcept
		{
			return const_cast<VirtualFileSystem *>(this)->FindNode(path);
		}
	};
} // namespace tudov
