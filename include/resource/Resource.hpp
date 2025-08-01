#pragma once

#include <span>
#include <vector>

namespace tudov
{
	struct IResource
	{
		virtual ~IResource() noexcept = default;

		virtual bool CanReadBytes() noexcept;

		virtual std::span<std::byte> ReadBytes();
	};
} // namespace tudov
