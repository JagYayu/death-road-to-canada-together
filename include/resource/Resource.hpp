#pragma once

#include <span>
#include <string_view>

namespace tudov
{
	struct IResource
	{
		virtual ~IResource() noexcept = default;

		/**
		 * Get the file path in Virtual File System.
		 */
		virtual std::string_view GetFilePath()const noexcept = 0;

		virtual bool CanReadBytes() noexcept;

		virtual std::span<std::byte> ReadBytes();
	};
} // namespace tudov
