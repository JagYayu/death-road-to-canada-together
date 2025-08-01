#pragma once

#include "util/Definitions.hpp"
#include "util/Log.hpp"

#include <filesystem>
#include <functional>
#include <map>
#include <vector>

namespace tudov
{
	template <typename T>
	class Resources
	{
	  public:
		struct Entry
		{
			std::string path;
			std::shared_ptr<T> resource;
		};

	  protected:
		std::shared_ptr<Log> _log;
		ResourceID _latestID = 0;

		std::map<std::string_view, ResourceID> _path2ID;
		std::map<ResourceID, Entry> _id2Entry;

	  public:
		explicit Resources() noexcept;
		virtual ~Resources() noexcept = default;

	  protected:
		inline virtual void OnUnloadResource(ResourceID id) noexcept
		{
		}

	  public:
		inline std::vector<std::reference_wrapper<const Entry>> ListResources(const std::filesystem::path &path) const
		{
			std::vector<std::reference_wrapper<const Entry>> resources{};
			std::string prefix = path.generic_string();
			auto [lower, upper] = _path2ID.equal_range(prefix);

			while (lower != _path2ID.end() && lower->first.starts_with(prefix))
			{
				ResourceID id = lower->second;
				const Entry &entry = _id2Entry.at(id);
				resources.emplace_back(std::reference_wrapper<const Entry>(entry));
				++lower;
			}

			return resources;
		}

		inline std::shared_ptr<T> GetResource(ResourceID id) const noexcept
		{
			auto &&it = _id2Entry.find(id);
			return it != _id2Entry.end() ? it->second.resource : nullptr;
		}

		inline std::string_view GetResourcePath(ResourceID id) const noexcept
		{
			auto &&it = _id2Entry.find(id);
			return it != _id2Entry.end() ? std::string_view(it->second.path) : "";
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

			if (auto &&it = _path2ID.find(path); it != _path2ID.end()) [[unlikely]]
			{
				throw std::runtime_error("Resource already been loaded");
			}

			++_latestID;
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

			_log->Trace("Loaded resource <{}>\"{}\"", id, path);

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

			_log->Trace("Unloaded resource <{}>\"{}\"", id, it->second.path);

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