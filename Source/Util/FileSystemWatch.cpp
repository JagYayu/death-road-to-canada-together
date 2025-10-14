/**
 * @file Util/FileSystemWatch.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Util/FileSystemWatch.hpp"

#include "FileWatch.hpp"
#include "Data/PathType.hpp"
#include "System/LogMicros.hpp"
#include <filesystem>
#include <stdexcept>

using namespace tudov;

using FileWatch = filewatch::FileWatch<std::string>;

FileSystemWatch::FileSystemWatch() noexcept
    : _fileWatch(nullptr)
{
}

FileSystemWatch::FileSystemWatch(const std::filesystem::path &path) noexcept
    : _fileWatch(nullptr),
      _path(path)
{
}

FileSystemWatch::~FileSystemWatch() noexcept
{
	StopWatching();
}

DelegateEvent<std::string_view, EPathType, EFileChangeType> &FileSystemWatch::GetOnCallback() noexcept
{
	return _onCallback;
}

std::filesystem::path FileSystemWatch::GetPath() const noexcept
{
	return _path;
}

void FileSystemWatch::SetPath(const std::filesystem::path &path) noexcept
{
	_path = path;
}

bool FileSystemWatch::IsWatching() const noexcept
{
	return _fileWatch != nullptr;
}

void FileSystemWatch::StartWatching()
{
	if (!std::filesystem::exists(_path)) [[unlikely]]
	{
		throw std::runtime_error("Path is not a valid!");
	}

	_fileWatch = new FileWatch(_path.generic_string(), [this](std::string_view path, const filewatch::Event changeType)
	{
		std::filesystem::file_type fileType;
		try
		{
			fileType = std::filesystem::status(std::filesystem::path(_path / path)).type();
		}
		catch (const std::filesystem::filesystem_error &e)
		{
			fileType = std::filesystem::file_type::none;
		}

		EPathType pathType;
		switch (fileType)
		{
		case std::filesystem::file_type::regular:
			pathType = EPathType::File;
			break;
		case std::filesystem::file_type::directory:
		case std::filesystem::file_type::junction:
			pathType = EPathType::Directory;
			break;
		case std::filesystem::file_type::none:
		case std::filesystem::file_type::not_found:
			pathType = EPathType::None;
			break;
		default:
			pathType = EPathType::Other;
			break;
		}

		_onCallback.Invoke(path, pathType, static_cast<EFileChangeType>(changeType));
	});
}

void FileSystemWatch::StopWatching()
{
	if (_fileWatch != nullptr)
	{
		delete static_cast<FileWatch *>(_fileWatch);
		_fileWatch = nullptr;
	}
}
