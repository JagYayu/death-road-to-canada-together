#pragma once

#include "util/Defs.h"
#include "util/Log.h"

// #include "Texture.h"

namespace tudov
{
	template <typename T>
	class ResourceManager
	{
	  private:
		struct Entry
		{
			std::string path;
			std::shared_ptr<T> resource;
		};

		std::shared_ptr<Log> _log;
		ResourceID _latestID = 0;

		std::unordered_map<std::string_view, ResourceID> _path2ID;
		std::unordered_map<ResourceID, Entry> _id2Entry;

	  public:
		explicit ResourceManager() noexcept;
		~ResourceManager() noexcept = default;

	  protected:
		inline virtual void OnUnloadResource(ResourceID id) noexcept
		{
		}

	  public:
		inline std::shared_ptr<T> GetResource(ResourceID id) const noexcept
		{
			auto &&it = _id2Entry.find(id);
			return it != _id2Entry.end() ? it->second.resource : nullptr;
		}

		inline ResourceID GetResourceID(std::string_view path) const noexcept
		{
			auto &&it = _path2ID.find(path);
			return it != _path2ID.end() ? it->second : false;
		}

		template <typename TDerived = T, typename... Args>
		inline ResourceID Load(std::string_view path, Args &&...args)
		{
			static_assert(std::is_base_of_v<T, TDerived>, "TDerived must inherit from T");

			_latestID++;
			auto id = _latestID;

			std::shared_ptr<T> resource;
			try
			{
				resource = std::static_pointer_cast<T>(std::make_shared<TDerived>(std::forward<Args>(args)...));
			}
			catch (const std::exception &e)
			{
				_log->Error("Exception occurred while loading \"{}\": {}", path, e.what());
			}

			Entry entry{
			    .path = std::string(path),
			    .resource = resource,
			};
			auto &&result = _id2Entry.try_emplace(id, entry);
			_path2ID[std::string_view(result.first->second.path)] = id;

			_log->Debug("Loaded resource <{}>\"{}\"", id, path);

			return id;
		}

		inline void Unload(ResourceID id) noexcept
		{
			auto &&it = _id2Entry.find(id);
			if (it == _id2Entry.end())
			{
				_log->Warn("Attempt to unload invalid resource <{}>", id);
				return;
			}

			_path2ID.erase(std::string_view(it->second.path));
			_id2Entry.erase(it);
		}

		inline void Clear() noexcept
		{
			_path2ID.clear();
			_id2Entry.clear();
		}
	};
} // namespace tudov