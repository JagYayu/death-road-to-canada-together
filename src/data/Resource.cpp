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
