#pragma once

namespace tudov
{
	struct IStorageManager;

	struct IStorage
	{
		virtual ~IStorage() noexcept = default;

		virtual bool CanRead() noexcept = 0;
		virtual bool CanWrite() noexcept = 0;
		virtual IStorageManager &GetStorageManager() noexcept = 0;

		inline const IStorageManager &GetStorageManager() const noexcept
		{
			return const_cast<IStorage *>(this)->GetStorageManager();
		}
	};
} // namespace tudov
