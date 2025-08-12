#include "data/VirtualFileSystem.hpp"

#include "data/PathType.hpp"
#include "util/EnumFlag.hpp"
#include "util/LogMicros.hpp"

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <variant>

using namespace tudov;

Log &VirtualFileSystem::GetLog() noexcept
{
	return *Log::Get("VirtualFileSystem");
}

DelegateEvent<const std::filesystem::path &> &VirtualFileSystem::GetOnMountDirectory() noexcept
{
	return _onMountDirectory;
}

DelegateEvent<const std::filesystem::path &> &VirtualFileSystem::GetOnDismountDirectory() noexcept
{
	return _onDismountDirectory;
}

DelegateEvent<const std::filesystem::path &, const std::vector<std::byte> &> &VirtualFileSystem::GetOnMountFile() noexcept
{
	return _onMountFile;
}

DelegateEvent<const std::filesystem::path &> &VirtualFileSystem::GetOnDismountFile() noexcept
{
	return _onDismountFile;
}

DelegateEvent<const std::filesystem::path &, const std::vector<std::byte> &, const std::vector<std::byte> &> &VirtualFileSystem::GetOnRemountFile() noexcept
{
	return _onRemountFile;
}

void VirtualFileSystem::MountDirectory(const std::filesystem::path &path) noexcept
{
	if (CanDebug())
	{
		Debug("Mount directory \"{}\"", path.generic_string());
	}

	DirectoryNode *currentDirectory = &std::get<DirectoryNode>(_rootNode);

	for (const auto &part : path)
	{
		auto partStr = part.generic_string();
		auto &children = currentDirectory->children;

		if (children.find(partStr) == children.end())
		{
			children[partStr] = DirectoryNode{};
		}

		currentDirectory = &std::get<DirectoryNode>(children[partStr]);
	}
}

void VirtualFileSystem::MountFile(const std::filesystem::path &path, std::vector<std::byte> bytes) noexcept
{
	if (CanDebug())
	{
		Debug("Mount file \"{}\"", path.generic_string());
	}

	DirectoryNode *currentDirectory = &std::get<DirectoryNode>(_rootNode);

	for (const auto &part : path)
	{
		auto partStr = part.generic_string();

		if (part.has_extension())
		{
			currentDirectory->children.emplace(partStr, FileNode{bytes});

			break;
		}

		auto &children = currentDirectory->children;

		if (children.find(partStr) == children.end())
		{
			children.emplace(partStr, DirectoryNode{});
		}

		currentDirectory = &std::get<DirectoryNode>(children[partStr]);
	}

	_onMountFile.Invoke(path, bytes);
}

bool VirtualFileSystem::DismountDirectory(const std::filesystem::path &path) noexcept
{
	if (CanDebug())
	{
		Debug("Dismount directory \"{}\"", path.generic_string());
	}

	DirectoryNode *currentDirectory = &std::get<DirectoryNode>(_rootNode);
	std::vector<std::string> parts;

	for (const auto &part : path)
	{
		parts.push_back(part.generic_string());
	}

	auto &children = currentDirectory->children;
	for (const auto &part : parts)
	{
		if (children.find(part) != children.end())
		{
			currentDirectory = &std::get<DirectoryNode>(children[part]);
		}
		else
		{
			return false;
		}
	}

	bool result = children.erase(parts.back());
	if (result)
	{
		_onDismountDirectory.Invoke(path);
	}
	return result;
}

bool VirtualFileSystem::DismountFile(const std::filesystem::path &path) noexcept
{
	if (CanDebug())
	{
		Debug("Dismount file \"{}\"", path.generic_string());
	}

	DirectoryNode *currentDirectory = &std::get<DirectoryNode>(_rootNode);

	auto itLast = path.end();
	--itLast;

	for (auto it = path.begin(); it != path.end(); ++it)
	{
		auto partStr = it->generic_string();
		auto &children = currentDirectory->children;

		if (children.find(partStr) == children.end())
		{
			return false;
		}

		if (it == itLast)
		{
			bool result = currentDirectory->children.erase(partStr);
			if (result)
			{
				_onDismountFile.Invoke(path);

				return true;
			}

			return result;
		}

		currentDirectory = &std::get<DirectoryNode>(children.at(partStr));
	}

	return false;
	// bool result = currentDirectory->children.erase(path.filename().generic_string());
	// if (result)
	// {
	// 	_onDismountFile.Invoke(path);
	// }
	// return result;
}

bool VirtualFileSystem::RemountFile(const std::filesystem::path &path, const std::vector<std::byte> &bytes) noexcept
{
	TE_DEBUG("Remount file \"{}\"", path.generic_string());

	Node *node = FindNode(path);
	if (node != nullptr && std::holds_alternative<FileNode>(*node))
	{
		auto &fileNode = std::get<FileNode>(*node);
		auto &oldBytes = fileNode.bytes;
		fileNode.bytes = bytes;

		_onRemountFile.Invoke(path, bytes, oldBytes);
		return true;
	}

	// MountFile(path, bytes);
	return false;
}

bool VirtualFileSystem::Exists(const std::filesystem::path &path) noexcept
{
	return FindNode(path) != nullptr;
}

EPathType VirtualFileSystem::GetPathType(const std::filesystem::path &path) noexcept
{
	const auto *node = FindNode(path);
	if (!node)
	{
		return EPathType::None;
	}

	if (std::holds_alternative<DirectoryNode>(*node))
	{
		return EPathType::Directory;
	}
	else if (std::holds_alternative<FileNode>(*node))
	{
		return EPathType::File;
	}

	return EPathType::None;
}

std::span<std::byte> VirtualFileSystem::GetFileBytes(const std::filesystem::path &path)
{
	Node *node = FindNode(path);
	if (node == nullptr) [[unlikely]]
	{
		throw std::runtime_error("file not found");
	}

	if (!std::holds_alternative<FileNode>(*node)) [[unlikely]]
	{
		throw std::runtime_error("path is not file");
	}

	return std::get<FileNode>(*node).bytes;
}

std::vector<std::string_view> VirtualFileSystem::List(const std::filesystem::path &directory, ListOption options)
{
	DirectoryNode *directoryNode;
	{
		Node *node = FindNode(directory);
		if (node == nullptr)
		{
			throw std::runtime_error("directory not found");
		}

		if (!std::holds_alternative<DirectoryNode>(*node)) [[unlikely]]
		{
			throw std::runtime_error("path is not a directory");
		}
		directoryNode = &std::get<DirectoryNode>(*node);
	}

	std::vector<std::string_view> result{};

	for (const auto &[path, child] : directoryNode->children)
	{
		if (std::holds_alternative<FileNode>(child))
		{
			if (EnumFlag::HasAny(options, ListOption::File))
			{
				result.push_back(path);
			}
		}
		else if (std::holds_alternative<DirectoryNode>(child))
		{
			if (EnumFlag::HasAny(options, ListOption::Directory))
			{
				result.push_back(path);
			}
		}
	}

	if (EnumFlag::HasAny(options, ListOption::Sorted))
	{
		std::sort(result.begin(), result.end());
	}

	return result;
}

VirtualFileSystem::Node *VirtualFileSystem::FindNode(const std::filesystem::path &path) noexcept
{
	Node *currentNode = &_rootNode;

	for (const auto &part : path)
	{
		auto partStr = part.generic_string();

		if (!std::holds_alternative<DirectoryNode>(*currentNode))
		{
			return nullptr;
		}

		auto &children = std::get<DirectoryNode>(*currentNode).children;
		if (children.find(partStr) == children.end())
		{
			return nullptr;
		}

		currentNode = &children.at(partStr);
	}

	return currentNode;
}
