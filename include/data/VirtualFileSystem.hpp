#pragma once

#include "event/DelegateEvent.hpp"
#include "util/Log.hpp"

#include <cstddef>
#include <filesystem>
#include <map>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace tudov
{
	enum class EPathType;

	class VirtualFileSystem : public ILogProvider
	{
	  public:
		enum class ListOption : char
		{
			All = -1,
			None = 0,
			File = 1 << 0,
			Directory = 1 << 1,
			Recursed = 1 << 2,
			Sorted = 1 << 3,
		};

	  private:
		struct DirectoryNode;
		struct FileNode;
		using Node = std::variant<DirectoryNode, FileNode>;

		struct DirectoryNode
		{
			std::map<std::string, Node> children;
		};

		struct FileNode
		{
			std::vector<std::byte> bytes;
		};

	  private:
		Node _rootNode;
		DelegateEvent<const std::filesystem::path &> _onMountDirectory;
		DelegateEvent<const std::filesystem::path &> _onDismountDirectory;
		DelegateEvent<const std::filesystem::path &, const std::vector<std::byte> &> _onMountFile;
		DelegateEvent<const std::filesystem::path &> _onDismountFile;
		DelegateEvent<const std::filesystem::path &, const std::vector<std::byte> &, const std::vector<std::byte> &> _onRemountFile;

	  public:
		explicit VirtualFileSystem() noexcept = default;
		~VirtualFileSystem() noexcept override = default;

		Log &GetLog() noexcept override;

		DelegateEvent<const std::filesystem::path &> &GetOnMountDirectory() noexcept;
		DelegateEvent<const std::filesystem::path &> &GetOnDismountDirectory() noexcept;
		DelegateEvent<const std::filesystem::path &, const std::vector<std::byte> &> &GetOnMountFile() noexcept;
		DelegateEvent<const std::filesystem::path &> &GetOnDismountFile() noexcept;
		DelegateEvent<const std::filesystem::path &, const std::vector<std::byte> &, const std::vector<std::byte> &> &GetOnRemountFile() noexcept;

		void MountDirectory(const std::filesystem::path &path) noexcept;
		void MountFile(const std::filesystem::path &path, std::vector<std::byte> bytes) noexcept;
		bool DismountDirectory(const std::filesystem::path &path) noexcept;
		bool DismountFile(const std::filesystem::path &path) noexcept;
		bool RemountFile(const std::filesystem::path &path, const std::vector<std::byte> &bytes) noexcept;

		bool Exists(const std::filesystem::path &path) noexcept;
		EPathType GetPathType(const std::filesystem::path &path) noexcept;
		std::span<std::byte> GetFileBytes(const std::filesystem::path &file);

		std::vector<std::string_view> List(const std::filesystem::path &directory, ListOption options = ListOption::All);

	  private:
		Node *FindNode(const std::filesystem::path &path) noexcept;

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
