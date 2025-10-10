/**
 * @file data/Resource.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "resource/Resource.hpp"

#include <stdexcept>

using namespace tudov;

bool IResource::CanReadBytes() noexcept
{
	return false;
}

std::span<std::byte> IResource::ReadBytes()
{
	throw std::runtime_error("This resource type does not support reading bytes");
}
