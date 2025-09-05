/**
 * @file data/VirtualFileSystem.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "data/VirtualFileSystem.hpp"

#include "data/PathType.hpp"
#include "mod/ScriptEngine.hpp"
#include "program/Tudov.hpp"
#include "resource/ResourceType.hpp"
#include "sol/string_view.hpp"
#include "system/LogMicros.hpp"
#include "util/EnumFlag.hpp"
#include "util/LuaUtils.hpp"
#include "util/Utils.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <variant>
#include <vector>
#include <winscard.h>

using namespace tudov;

#pragma region VirtualFileSystem::List

bool VirtualFileSystem::ListEntry::operator<(const ListEntry &r) const noexcept
{
	if (isDirectory != r.isDirectory)
	{
		return isDirectory > r.isDirectory;
	}
	return path < r.path;
}

#pragma endregion

VirtualFileSystem::CommonNode::CommonNode() noexcept
    : date(std::chrono::system_clock::now())
{
}

VirtualFileSystem::DirectoryNode::DirectoryNode(const std::map<std::string, Node> &children) noexcept
    : children(children),
      cached(false),
      cachedSize(0),
      CommonNode()
{
}

size_t VirtualFileSystem::DirectoryNode::GetSize() const noexcept
{
	if (!cached) [[unlikely]]
	{
		cachedSize = 0;

		for (auto &[childPath, childNode] : children)
		{
			if (std::holds_alternative<FileNode>(childNode))
			{
				cachedSize += std::get<FileNode>(childNode).bytes.size();
			}
			else if (std::holds_alternative<DirectoryNode>(childNode))
			{
				cachedSize += std::get<DirectoryNode>(childNode).GetSize();
			}
			else [[unlikely]]
			{
			}
		}
	}

	return cachedSize;
}

VirtualFileSystem::FileNode::FileNode(const std::vector<std::byte> &bytes, EResourceType resourceType) noexcept
    : bytes(bytes),
      resourceType(resourceType),
      CommonNode()
{
}

#pragma region VirtualFileSystem

VirtualFileSystem::VirtualFileSystem(Context &context) noexcept
    : _context(context),
      _rootNode()
{
}

Context &VirtualFileSystem::GetContext() noexcept
{
	return _context;
}

Log &VirtualFileSystem::GetLog() noexcept
{
	return *Log::Get("VirtualFileSystem");
}

VirtualFileSystem::MountDirectoryEvent &VirtualFileSystem::GetOnMountDirectory() noexcept
{
	return _onMountDirectory;
}

VirtualFileSystem::DismountDirectoryEvent &VirtualFileSystem::GetOnDismountDirectory() noexcept
{
	return _onDismountDirectory;
}

VirtualFileSystem::MountFileEvent &VirtualFileSystem::GetOnMountFile() noexcept
{
	return _onMountFile;
}

VirtualFileSystem::DismountFileEvent &VirtualFileSystem::GetOnDismountFile() noexcept
{
	return _onDismountFile;
}

VirtualFileSystem::RemountFileEvent &VirtualFileSystem::GetOnRemountFile() noexcept
{
	return _onRemountFile;
}

void VirtualFileSystem::MountDirectory(const std::filesystem::path &path) noexcept
{
	TE_DEBUG("Mount directory \"{}\"", path.generic_string());

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

void VirtualFileSystem::MountFile(const std::filesystem::path &path) noexcept
{
	std::basic_ifstream<std::byte> stream{path.generic_string(), std::ios::binary};
	if (!stream)
	{
		TE_ERROR("Failed to mount file \"{}\": failed to open in file stream", path.generic_string());
	}

	std::vector<std::byte> bytes{std::istreambuf_iterator<std::byte>(stream), std::istreambuf_iterator<std::byte>()};
	MountFile(path, bytes);
}

void VirtualFileSystem::MountFile(const std::filesystem::path &path, std::span<const std::byte> bytes) noexcept
{
	TE_DEBUG("Mount file \"{}\"", path.generic_string());

	DirectoryNode *currentDirectory = &std::get<DirectoryNode>(_rootNode);

	bool mounted = false;
	for (const auto &part : path)
	{
		const std::string &partStr = part.generic_string();

		if (part.has_extension()) // TODO or this is the last part, then we assume it is a file ... right?
		{
			auto result = currentDirectory->children.emplace(
			    partStr,
			    FileNode(std::vector<std::byte>(bytes.begin(), bytes.end()), EResourceType::Unknown));
			if (!result.second) [[unlikely]]
			{
				TE_FATAL("Failed to add child node \"{}\"", partStr);
			}

			auto &fileNode_ = std::get<FileNode>(result.first->second);
			_onMountFile.Invoke(path, fileNode_.bytes, fileNode_.resourceType);
			ClearParentDirectoriesCache(path);

			break;
		}

		auto &children = currentDirectory->children;

		if (children.find(partStr) == children.end())
		{
			children.emplace(partStr, DirectoryNode());
		}

		currentDirectory = &std::get<DirectoryNode>(children[partStr]);
	}
}

bool VirtualFileSystem::DismountDirectory(const std::filesystem::path &path) noexcept
{
	TE_DEBUG("Dismount directory \"{}\"", path.generic_string());

	DirectoryNode *currentDirectory = &std::get<DirectoryNode>(_rootNode);
	std::vector<std::string> parts;

	for (const auto &part : path)
	{
		parts.push_back(part.generic_string());
	}

	auto &children = currentDirectory->children;
	for (const auto &part : parts)
	{
		if (children.find(part) == children.end())
		{
			return false;
		}

		currentDirectory = &std::get<DirectoryNode>(children[part]);
	}

	if (children.erase(parts.back()))
	{
		_onDismountDirectory.Invoke(path);

		return true;
	}
	return false;
}

bool VirtualFileSystem::DismountFile(const std::filesystem::path &path) noexcept
{
	TE_DEBUG("Dismount file \"{}\"", path.generic_string());

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
		fileNode.resourceType = EResourceType::Unknown;

		_onRemountFile.Invoke(path, fileNode.bytes, oldBytes, fileNode.resourceType);
		ClearParentDirectoriesCache(path);

		return true;
	}

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
	else
	{
		return EPathType::Other;
	}
}

std::chrono::time_point<std::chrono::system_clock> VirtualFileSystem::GetPathDateModified(const std::filesystem::path &path)
{
	const auto *node = FindNode(path);
	if (!node)
	{
		throw std::runtime_error("path not found");
	}

	if (std::holds_alternative<DirectoryNode>(*node))
	{
		return std::get<DirectoryNode>(*node).date;
	}
	else if (std::holds_alternative<FileNode>(*node))
	{
		return std::get<FileNode>(*node).date;
	}
	else [[unlikely]]
	{
		throw std::runtime_error("Invalid node");
	}
}

std::span<std::byte> VirtualFileSystem::GetFileBytes(const std::filesystem::path &file)
{
	Node *node = FindNode(file);
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

EResourceType VirtualFileSystem::GetFileResourceType(const std::filesystem::path &file)
{
	Node *node = FindNode(file);
	if (node == nullptr) [[unlikely]]
	{
		throw std::runtime_error("file not found");
	}

	if (!std::holds_alternative<FileNode>(*node)) [[unlikely]]
	{
		throw std::runtime_error("path is not file");
	}

	return std::get<FileNode>(*node).resourceType;
}

std::size_t VirtualFileSystem::GetPathSize(const std::filesystem::path &file) const
{
	const Node *node = FindNode(file);
	if (node == nullptr) [[unlikely]]
	{
		throw std::runtime_error("file not found");
	}

	if (std::holds_alternative<FileNode>(*node))
	{
		return std::get<FileNode>(*node).bytes.size();
	}
	else if (std::holds_alternative<DirectoryNode>(*node))
	{
		return std::get<DirectoryNode>(*node).GetSize();
	}
	else [[unlikely]]
	{
		return 0;
	}
}

std::vector<VirtualFileSystem::ListEntry> VirtualFileSystem::List(const std::filesystem::path &directory, EPathListOption options) const
{
	const DirectoryNode *directoryNode;
	{
		const Node *node = FindNode(directory);
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

	std::vector<ListEntry> result{};

	if (EnumFlag::HasAny(options, EPathListOption::FullPath))
	{
		CollectListEntries(result, directory, directoryNode, options, 255);
	}
	else
	{
		CollectListEntries(result, "", directoryNode, options, 255);
	}

	if (EnumFlag::HasAny(options, EPathListOption::Sorted))
	{
		std::sort(result.begin(), result.end());
	}

	return result;
}

void VirtualFileSystem::CollectListEntries(std::vector<ListEntry> &entries, const std::filesystem::path &directory, const DirectoryNode *directoryNode, EPathListOption options, std::uint32_t depth) const noexcept
{
	for (const auto &[relativePath, child] : directoryNode->children)
	{
		std::filesystem::path path = directory / relativePath;

		if (std::holds_alternative<FileNode>(child))
		{
			if (EnumFlag::HasAny(options, EPathListOption::File))
			{
				entries.emplace_back(path.generic_string(), false);
			}
		}
		else if (std::holds_alternative<DirectoryNode>(child))
		{
			if (EnumFlag::HasAny(options, EPathListOption::Directory))
			{
				entries.emplace_back(path.generic_string(), true);
			}

			if (EnumFlag::HasAny(options, EPathListOption::Recursed) && depth != 0)
			{
				const DirectoryNode *childDirectoryNode = &std::get<DirectoryNode>(child);
				CollectListEntries(entries, path, childDirectoryNode, options, depth - 1);
			}
		}
	}
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

void VirtualFileSystem::ClearParentDirectoriesCache(const std::filesystem::path &path) const
{
	std::filesystem::path directory = path.parent_path();

	while (!directory.empty() && directory != directory.root_path())
	{
		auto *node = FindNode(directory);
		TE_ASSERT(node != nullptr);
		TE_ASSERT(std::holds_alternative<DirectoryNode>(*node));
		auto directoryNode = std::get<DirectoryNode>(*node);
		directoryNode.cached = false;

		directory = directory.parent_path();
	}
}

bool VirtualFileSystem::LuaExists(sol::object path)
{
	std::filesystem::path path_;
	if (path.is<sol::string_view>())
	{
		path_ = std::filesystem::path(path.as<sol::string_view>());
	}
	else [[unlikely]]
	{
		LuaUtils::Deconstruct(path_);
		throw std::runtime_error(std::format("bad argument #2 to 'path' (string expected, got {})", GetLuaTypeStringView(path.get_type())));
	}

	return Exists(path_);
}

sol::table VirtualFileSystem::LuaList(sol::object directory, sol::object options)
{
	try
	{
		IScriptEngine &scriptEngine = GetScriptEngine();

		std::vector<ListEntry> list;
		{
			std::filesystem::path directory_;
			if (directory.is<sol::string_view>())
			{
				directory_ = std::filesystem::path(directory.as<sol::string_view>());
			}
			else if (directory.is<sol::nil_t>())
			{
				directory_ = "";
			}
			else [[unlikely]]
			{
				LuaUtils::Deconstruct(list, directory_);
				scriptEngine.ThrowError("bad argument #2 to 'directory' (string or nil expected, got {})", GetLuaTypeStringView(directory.get_type()));
			}

			EPathListOption options_;
			if (options.is<std::double_t>())
			{
				options_ = static_cast<EPathListOption>(options.as<std::double_t>());
			}
			else if (options.is<sol::nil_t>())
			{
				options_ = EPathListOption::Default;
			}
			else [[unlikely]]
			{
				LuaUtils::Deconstruct(list, directory_, options_);
				throw std::runtime_error(std::format("bad argument #3 to 'directory' (number expected, got {})", GetLuaTypeStringView(options.get_type())));
			}

			list = List(directory_, options_);
		}

		sol::table result = scriptEngine.CreateTable(list.size(), 0);
		std::size_t index = 0;
		for (const ListEntry &entry : list)
		{
			++index;

			sol::table tbl = scriptEngine.CreateTable(0, 2);
			tbl["path"] = entry.path;
			tbl["isDirectory"] = entry.isDirectory;

			result[index] = tbl;
		}

		return result;
	}
	catch (const std::exception &e)
	{
		std::string errorMessage = e.what();
		LuaUtils::Deconstruct(e);
		GetScriptEngine().ThrowError(errorMessage);
	}

	return GetScriptEngine().CreateTable();
}

std::string VirtualFileSystem::LuaReadFile(sol::object file)
{
	try
	{
		std::filesystem::path file_;
		if (file.is<sol::string_view>())
		{
			file_ = std::filesystem::path(file.as<sol::string_view>());
		}
		else if (file.is<sol::nil_t>())
		{
			file_ = "";
		}
		else [[unlikely]]
		{
			LuaUtils::Deconstruct(file_);
			GetScriptEngine().ThrowError("bad argument #2 to 'file' (string or nil expected, got {})", GetLuaTypeStringView(file.get_type()));
		}

		std::span<std::byte> bytes = GetFileBytes(file_);
		return std::string(reinterpret_cast<const char *>(bytes.data()), bytes.size());
	}
	catch (const std::exception &e)
	{
		std::string errorMessage = e.what();
		LuaUtils::Deconstruct(e);
		GetScriptEngine().ThrowError(errorMessage);
	}

	return "";
}

#pragma endregion VirtualFileSystem
