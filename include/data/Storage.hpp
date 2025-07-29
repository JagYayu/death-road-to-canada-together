#pragma once

#include "util/Log.hpp"

#include <functional>

struct SDL_Storage;

namespace tudov
{
	struct IStorageManager;

	struct IStorage
	{
		enum class EEnumerationResult;

		template <typename TCallbackArgs = void *>
		using EnumerationCallbackFunction = std::function<EEnumerationResult(TCallbackArgs, std::string_view, std::string_view)>;

		virtual ~IStorage() noexcept = default;

		virtual IStorageManager &GetStorageManager() noexcept = 0;
		virtual bool CanRead() noexcept = 0;
		virtual bool CanWrite() noexcept = 0;
		virtual bool IsReady() noexcept = 0;
		virtual void EnumerateDirectory(std::string_view path, const EnumerationCallbackFunction<> &callback, void *callbackArgs) noexcept = 0;

		inline const IStorageManager &GetStorageManager() const noexcept
		{
			return const_cast<IStorage *>(this)->GetStorageManager();
		}

		template <typename TCallbackArgs>
		inline void EnumerateDirectory(std::string_view path, const EnumerationCallbackFunction<TCallbackArgs *> &callback, TCallbackArgs *callbackArgs) noexcept
		{
			EnumerateDirectory(path, callback, callbackArgs);
		}
	};

	class Storage : public virtual IStorage, public ILogProvider
	{
	  protected:
		SDL_Storage *_storage;

	  public:
		explicit Storage() noexcept;
		virtual ~Storage() noexcept;

		bool IsReady() noexcept override;
		bool CanRead() noexcept override;
		void EnumerateDirectory(std::string_view path, const EnumerationCallbackFunction<> &callback, void *callbackArgs) noexcept override;
	};
} // namespace tudov
