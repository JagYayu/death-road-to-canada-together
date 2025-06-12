#pragma once

#include "util/Defs.h"
#include "util/Log.h"

// #include "Texture.h"

namespace tudov
{
	using ResourceID = UInt64;

	template <typename T>
	class ResourceManager
	{
		// using T = Texture;
	  private:
		struct Entry
		{
			String path;
			SharedPtr<T> resource;
		};

		Log _log;
		ResourceID _latestID = 0;

		UnorderedMap<StringView, ResourceID> _path2ID;
		UnorderedMap<ResourceID, Entry> _id2Entry;

	  public:
		explicit ResourceManager();

		inline SharedPtr<T> GetResource(ResourceID id) const noexcept
		{
			auto &&it = _id2Entry.find(id);
			return it != _id2Entry.end() ? it->second.resource : nullptr;
		}

		inline ResourceID GetResourceID(StringView path) const noexcept
		{
			auto &&it = _path2ID.find(path);
			return it != _id2Entry.end() ? it->first : false;
		}

		template <typename... Args>
		inline ResourceID Load(StringView path, Args &&...args)
		{
			_latestID++;
			auto id = _latestID;

			SharedPtr<T> resource;
			try
			{
				resource = MakeShared<T>(Forward<Args>(args)...);
			}
			catch (const std::exception &e)
			{
				_log.Error("Exception occurred while loading \"{}\": {}", path, e.what());
			}

			Entry entry{
			    .path = String(path),
			    .resource = resource,
			};

			_id2Entry.emplace(id, entry);
			_path2ID[StringView(entry.path)] = id;

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