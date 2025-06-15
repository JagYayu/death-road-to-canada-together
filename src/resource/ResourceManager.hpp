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
			SharedPtr<T> resource;
		};

		SharedPtr<Log> _log;
		ResourceID _latestID = 0;

		std::unordered_map<std::string_view, ResourceID> _path2ID;
		std::unordered_map<ResourceID, Entry> _id2Entry;

	  public:
		explicit ResourceManager() noexcept;

		inline SharedPtr<T> GetResource(ResourceID id) const noexcept
		{
			auto &&it = _id2Entry.find(id);
			return it != _id2Entry.end() ? it->second.resource : nullptr;
		}

		inline ResourceID GetResourceID(std::string_view path) const noexcept
		{
			auto &&it = _path2ID.find(path);
			return it != _id2Entry.end() ? it->first : false;
		}

		template <typename TDerived = T, typename... Args>
		inline ResourceID Load(std::string_view path, Args &&...args)
		{
			static_assert(std::is_base_of_v<T, TDerived>, "TDerived must inherit from T");

			_latestID++;
			auto id = _latestID;

			SharedPtr<T> resource;
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

			_id2Entry.emplace(id, entry);
			_path2ID[std::string_view(entry.path)] = id;

			return id;
		}

		inline void Unload(ResourceID id) noexcept
		{
			_id2Entry.erase(id);
		}

		inline void Clear() noexcept
		{
			_id2Entry.clear();
		}
	};
} // namespace tudov