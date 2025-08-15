#pragma once

#include "event/DelegateEvent.hpp"
#include "resource/ResourceType.hpp"
#include "util/Log.hpp"

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

	class VirtualFileSystem : private ILogProvider
	{
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

		enum class ListOption : std::uint8_t
		{
			All = static_cast<std::uint8_t>(-1),
			None = 0,
			File = 1 << 0,
			Directory = 1 << 1,
			Recursed = 1 << 2,
			Sorted = 1 << 3,
			FullPath = 1 << 4,
		};

	  private:
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

			explicit DirectoryNode(const std::map<std::string, Node> &children = {}) noexcept;
		};

		struct FileNode : public CommonNode
		{
			std::vector<std::byte> bytes;
			EResourceType resourceType;

			explicit FileNode(const std::vector<std::byte> &bytes = {}, EResourceType resourceType = EResourceType::Unknown) noexcept;
		};

	  private:
		Node _rootNode;
		MountDirectoryEvent _onMountDirectory;
		DismountDirectoryEvent _onDismountDirectory;
		MountFileEvent _onMountFile;
		DismountFileEvent _onDismountFile;
		RemountFileEvent _onRemountFile;

	  public:
		explicit VirtualFileSystem() noexcept;
		~VirtualFileSystem() noexcept override = default;

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

		std::vector<ListEntry> List(const std::filesystem::path &directory, ListOption options = ListOption::All) const;

	  private:
		Node *FindNode(const std::filesystem::path &path) noexcept;
		void CollectListEntries(std::vector<ListEntry> &entries, const std::filesystem::path &directory, const DirectoryNode *directoryNode, ListOption options, std::uint32_t depth) const noexcept;

	  public:
		inline std::span<const std::byte> GetFileBytes(const std::filesystem::path &path) const
		{
			return const_cast<VirtualFileSystem *>(this)->GetFileBytes(path);
		}

	  private:
		inline const Node *FindNode(const std::filesystem::path &path) const noexcept
		{
			return const_cast<VirtualFileSystem *>(this)->FindNode(path);
		}
	};
} // namespace tudov
