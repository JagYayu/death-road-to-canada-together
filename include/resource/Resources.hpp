/**
 * @file resource/Resources.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "system/Log.hpp"
#include "system/LogMicros.hpp"
#include "util/Definitions.hpp"

#include <stdexcept>
#include <unordered_map>

namespace tudov
{
	template <typename TResource>
	class Resources : private ILogProvider
	{
	  public:
		struct Entry
		{
			std::string path;
			std::shared_ptr<TResource> resource;
		};

	  protected:
		std::shared_ptr<Log> _log;
		ResourceID _latestID = 0;

		std::unordered_map<std::string_view, ResourceID> _path2ID;
		std::unordered_map<ResourceID, Entry> _id2Entry;

	  public:
		explicit Resources() noexcept;
		virtual ~Resources() noexcept = default;

	  protected:
		inline virtual void OnUnloadResource(ResourceID id) noexcept
		{
		}

	  public:
		Log &GetLog() noexcept override
		{
			return *_log;
		};

		inline std::shared_ptr<TResource> GetResource(ResourceID id) const noexcept
		{
			auto it = _id2Entry.find(id);
			return it != _id2Entry.end() ? it->second.resource : nullptr;
		}

		inline std::shared_ptr<TResource> GetResource(std::string_view path) const noexcept
		{
			return GetResource(GetResourceID(path));
		}

		inline std::string_view GetResourcePath(ResourceID id) const noexcept
		{
			auto it = _id2Entry.find(id);
			return it != _id2Entry.end() ? std::string_view(it->second.path) : "";
		}

		inline ResourceID GetResourceID(std::string_view path) const noexcept
		{
			auto it = _path2ID.find(path);
			return it != _path2ID.end() ? it->second : 0;
		}

		template <typename TDerived = TResource, typename... TArgs>
		inline ResourceID Load(std::string_view path, TArgs &&...args)
		{
			static_assert(std::is_base_of_v<TResource, TDerived>, "TDerived must inherit from T");

			if (auto it = _path2ID.find(path); it != _path2ID.end()) [[unlikely]]
			{
				throw std::runtime_error("Resource already been loaded");
			}

			++_latestID;
			ResourceID id = _latestID;

			std::shared_ptr<TResource> resource;
			try
			{
				resource = std::static_pointer_cast<TResource>(std::make_shared<TDerived>(path, std::forward<TArgs>(args)...));
			}
			catch (const std::exception &e)
			{
				TE_ERROR("Exception occurred while loading \"{}\": {}", path, e.what());
			}

			_id2Entry[id] = Entry{
			    .path = std::string(path),
			    .resource = resource,
			};
			_path2ID[_id2Entry[id].path] = id;

			TE_TRACE("Loaded resource <{}>{}", id, path);

			return id;
		}

		inline void Unload(ResourceID id) noexcept
		{
			auto it = _id2Entry.find(id);
			if (it == _id2Entry.end())
			{
				TE_WARN("Attempt to unload invalid resource <{}>", id);
				return;
			}

			TE_TRACE("Unloaded resource <{}>{}\"", id, it->second.path);

			_path2ID.erase(std::string_view(it->second.path));
			_id2Entry.erase(it);
		}

		inline void Unload(std::string_view path) noexcept
		{
			Unload(GetResourceID(path));
		}

		inline void UnloadAllFromDirectory(std::string_view directory)
		{
			//
		}

		inline void Clear() noexcept
		{
			_path2ID.clear();
			_id2Entry.clear();
		}
	};
} // namespace tudov