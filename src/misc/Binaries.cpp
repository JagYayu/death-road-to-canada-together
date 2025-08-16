/**
 * @file misc/Binaries.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "misc/Binaries.hpp"

using namespace tudov;

Binaries::Binaries(std::string_view path, const std::vector<std::byte> &bytes) noexcept
    : _path(path),
      _bytes(bytes.begin(), bytes.end())
{
}

std::string_view Binaries::GetFilePath() const noexcept
{
	return _path;
}

std::span<const std::byte> Binaries::GetBytes() const noexcept
{
	return _bytes;
}

std::uint64_t Binaries::GetSize() const noexcept
{
	return _bytes.size();
}
