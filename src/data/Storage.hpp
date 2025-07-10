#pragma once

namespace tudov
{
	struct IStorage
	{
		virtual ~IStorage() noexcept = default;

		virtual bool CanRead() noexcept = 0;
		virtual bool CanWrite() noexcept = 0;
	};
} // namespace tudov
