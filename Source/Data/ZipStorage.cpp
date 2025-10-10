/**
 * @file data/ZipStorage.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Data/ZipStorage.hpp"

#include "Data/HierarchyIterationResult.hpp"
#include "Data/PathType.hpp"

#include <filesystem>
#include <functional>
#include <minizip/ioapi.h>
#include <minizip/unzip.h>

#include <span>
#include <stdexcept>
#include <vector>

using namespace tudov;

struct MemoryBuffer
{
	const char *base;
	size_t size;
	size_t position;
};

static voidpf MemOpen(voidpf opaque, const char *filename, int mode)
{
	auto *mem = static_cast<MemoryBuffer *>(opaque);
	mem->position = 0;
	return opaque;
}

static uLong MemRead(voidpf opaque, voidpf stream, void *buf, uLong size)
{
	auto *mem = static_cast<MemoryBuffer *>(opaque);
	uLong remaining = static_cast<uLong>(mem->size - mem->position);
	uLong readSize = (size < remaining) ? size : remaining;
	std::memcpy(buf, mem->base + mem->position, readSize);
	mem->position += readSize;
	return readSize;
}

static long MemTell(voidpf opaque, voidpf stream)
{
	auto *mem = static_cast<MemoryBuffer *>(opaque);
	return static_cast<long>(mem->position);
}

static long MemSeek(voidpf opaque, voidpf stream, uLong offset, int origin)
{
	auto *mem = static_cast<MemoryBuffer *>(opaque);
	size_t newpos;
	switch (origin)
	{
	case ZLIB_FILEFUNC_SEEK_CUR:
		newpos = mem->position + offset;
		break;
	case ZLIB_FILEFUNC_SEEK_END:
		newpos = mem->size + offset;
		break;
	case ZLIB_FILEFUNC_SEEK_SET:
		newpos = offset;
		break;
	default:
		return -1;
	}

	if (newpos > mem->size)
		return -1;

	mem->position = newpos;
	return 0;
}

static int MemClose(voidpf opaque, voidpf stream)
{
	return 0;
}

static int MemError(voidpf opaque, voidpf stream)
{
	return 0;
}

void FillMemoryFileFunc(std::vector<std::byte> &data, zlib_filefunc_def *filefuncs)
{
	static MemoryBuffer static_buffer;
	static_buffer.base = reinterpret_cast<char *>(data.data());
	static_buffer.size = data.size();
	static_buffer.position = 0;

	filefuncs->zopen_file = MemOpen;
	filefuncs->zread_file = MemRead;
	filefuncs->ztell_file = MemTell;
	filefuncs->zseek_file = MemSeek;
	filefuncs->zclose_file = MemClose;
	filefuncs->zerror_file = MemError;
	filefuncs->opaque = &static_buffer;
}

ZipStorage::ZipStorage(std::span<std::byte> data)
    : _bytes(data.begin(), data.end())
{
	zlib_filefunc_def filefuncDef{};
	FillMemoryFileFunc(_bytes, &filefuncDef);

	_unzip = unzOpen2(nullptr, &filefuncDef);
	if (!_unzip)
	{
		throw std::runtime_error("Failed to open ZIP from memory.");
	}
}

ZipStorage::~ZipStorage() noexcept
{
}

bool ZipStorage::CanRead() noexcept
{
	return true;
}

bool ZipStorage::CanWrite() noexcept
{
	return true;
}

bool ZipStorage::IsReady() noexcept
{
	return true;
}

EHierarchyIterationResult ZipStorage::Foreach(const std::filesystem::path &directory, const EnumerationCallbackFunction<> &callback, void *callbackArgs) noexcept
{
	if (unzGoToFirstFile(_unzip) != UNZ_OK)
	{
		return EHierarchyIterationResult::Failure;
	}

	do
	{
		char fileInZip[256];
		unz_file_info64 info;
		auto result = unzGetCurrentFileInfo64(
		    _unzip,
		    &info,
		    fileInZip,
		    sizeof(fileInZip),
		    nullptr,
		    0,
		    nullptr,
		    0);

		if (result != UNZ_OK)
		{
			continue;
		}

		std::string fillPath{fileInZip};
		if (fillPath.find(directory.generic_string()) == 0)
		{
			EHierarchyIterationResult result = callback(fillPath, directory, callbackArgs);
			if (result != EHierarchyIterationResult::Continue)
			{
				return result;
			}
		}
	} while (unzGoToNextFile(_unzip) == UNZ_OK);

	return EHierarchyIterationResult::Continue;
}

EHierarchyElement ZipStorage::Check(const std::filesystem::path &path) noexcept
{
	if (unzLocateFile(_unzip, path.generic_string().c_str(), true) == UNZ_OK)
	{
		return EHierarchyElement::Data;
	}
	return EHierarchyElement::None;
}

PathInfo ZipStorage::GetPathInfo(const std::filesystem::path &path) noexcept
{
	// TODO throw std::runtime_error("NOT IMPLEMENT YET");
	return {};
}

std::uint64_t ZipStorage::GetPathSize(const std::filesystem::path &filePath) noexcept
{
	if (unzLocateFile(_unzip, filePath.generic_string().c_str(), 1) != UNZ_OK)
	{
		return 0;
	}

	unz_file_info64 file_info;
	if (unzGetCurrentFileInfo64(_unzip, &file_info, nullptr, 0, nullptr, 0, nullptr, 0) != UNZ_OK)
	{
		return 0;
	}

	return file_info.uncompressed_size;
}

EPathType ZipStorage::GetPathType(const std::filesystem::path &filePath) noexcept
{
	if (unzLocateFile(_unzip, filePath.generic_string().c_str(), 1) != UNZ_OK)
	{
		return EPathType::None;
	}

	std::filesystem::path pPath = filePath;

	if (pPath.has_extension())
	{
		return EPathType::File;
	}
	else
	{
		return EPathType::Directory;
	}
}

std::vector<std::byte> ZipStorage::ReadFileToBytes(const std::filesystem::path &filePath)
{
	if (unzLocateFile(_unzip, filePath.generic_string().c_str(), true) != UNZ_OK)
	{
		throw std::runtime_error("File not found in zip");
	}

	if (unzOpenCurrentFile(_unzip) != UNZ_OK)
	{
		throw std::runtime_error("Failed to open file in zip");
	}

	unz_file_info64 info;
	if (unzGetCurrentFileInfo64(_unzip, &info, nullptr, 0, nullptr, 0, nullptr, 0) != UNZ_OK)
	{
		unzCloseCurrentFile(_unzip);
		throw std::runtime_error("Failed to get file info");
	}

	if (info.uncompressed_size == 0)
	{
		unzCloseCurrentFile(_unzip);
		return {};
	}

	std::vector<std::byte> result{info.uncompressed_size};

	auto bytesRead = unzReadCurrentFile(_unzip, result.data(), static_cast<unsigned int>(result.size()));
	unzCloseCurrentFile(_unzip);

	if (bytesRead < 0 || static_cast<std::uint64_t>(bytesRead) != info.uncompressed_size)
	{
		throw std::runtime_error("Failed to read complete file content");
	}

	return result;
}
