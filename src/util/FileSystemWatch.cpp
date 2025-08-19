/**
 * @file util/FileSystemWatch.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "util/FileSystemWatch.hpp"

#include "FileWatch.hpp"
#include <filesystem>
#include <stdexcept>

using namespace tudov;

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

DelegateEvent<std::string_view, EFileChangeType> &FileSystemWatch::GetOnCallback() noexcept
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

	_fileWatch = new filewatch::FileWatch<std::string>(_path.generic_string(), [this](std::string_view path, const filewatch::Event changeType)
	{
		_onCallback.Invoke(path, static_cast<EFileChangeType>(changeType));
	});
}

void FileSystemWatch::StopWatching()
{
	if (_fileWatch != nullptr)
	{
		delete static_cast<filewatch::FileWatch<std::filesystem::path> *>(_fileWatch);
		_fileWatch = nullptr;
	}
}
