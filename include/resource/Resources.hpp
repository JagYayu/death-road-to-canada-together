#pragma once

#include "util/Definitions.hpp"
#include "util/Log.hpp"

#include <stdexcept>
#include <unordered_map>

namespace tudov
{
	template <typename TResource>
	class Resources
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
		// inline EHierarchyElement Check(const Path &path) noexcept override
		// {
		// 	if (IsData(path))
		// 		return EHierarchyElement::Data;
		// 	if (IsDirectory(path))
		// 		return EHierarchyElement::Directory;
		// 	return EHierarchyElement::None;
		// }

		// inline bool IsData(const Path &path) noexcept override
		// {
		// 	std::string pathStr = path.generic_string();
		// 	auto &&it = _path2ID.find(pathStr.data());
		// 	return it != _path2ID.end();
		// }

		// inline bool IsDirectory(const Path &path) noexcept override
		// {
		// 	std::string pathStr = path.generic_string();
		// 	auto &&it = std::find_if(_path2ID.begin(), _path2ID.end(), [&pathStr](const auto &entry)
		// 	{
		// 		auto &&[itSub, _] = std::mismatch(pathStr.begin(), pathStr.end(), entry.first.begin(), entry.first.end());
		// 		return itSub == pathStr.end();
		// 	});
		// 	return it != _path2ID.end();
		// }

		// inline bool IsNone(const Path &path) noexcept override
		// {
		// 	return !(IsNone(path) || IsDirectory(path));
		// }

		// TResource &Get(const Path &dataPath) override
		// {
		// 	std::string pathStr = dataPath.generic_string();
		// 	ResourceID id = GetResourceID(pathStr.data());
		// 	if (id == 0)
		// 	{
		// 		throw std::runtime_error("Resource not found");
		// 	}

		// 	return *GetResource(id);
		// }

		// inline EHierarchyIterationResult Foreach(const Path &directory, const IHierarchy<TResource &>::IterationCallback &callback, void *callbackArgs = nullptr) override
		// {
		// 	std::string prefix = directory.generic_string();
		// 	auto [lower, upper] = _path2ID.equal_range(prefix);

		// 	while (lower != _path2ID.end() && lower->first.starts_with(prefix))
		// 	{
		// 		auto path = Path(lower->first);
		// 		ResourceID id = lower->second;
		// 		const Entry &entry = _id2Entry.at(id);
		// 		EHierarchyIterationResult result = callback(path, directory, callbackArgs);

		// 		if (result != EHierarchyIterationResult::Continue)
		// 		{
		// 			return result;
		// 		}
		// 		++lower;
		// 	}

		// 	return EHierarchyIterationResult::Continue;
		// }

		// inline std::vector<std::reference_wrapper<const Entry>> ListResources(const std::filesystem::path &path) const
		// {
		// 	std::vector<std::reference_wrapper<const Entry>> resources{};
		// 	std::string prefix = path.generic_string();
		// 	auto [lower, upper] = _path2ID.equal_range(prefix);

		// 	while (lower != _path2ID.end() && lower->first.starts_with(prefix))
		// 	{
		// 		ResourceID id = lower->second;
		// 		const Entry &entry = _id2Entry.at(id);
		// 		resources.emplace_back(std::reference_wrapper<const Entry>(entry));
		// 		++lower;
		// 	}

		// 	return resources;
		// }

		inline std::shared_ptr<TResource> GetResource(ResourceID id) const noexcept
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

		template <typename TDerived = TResource, typename... Args>
		inline ResourceID Load(std::string_view path, Args &&...args)
		{
			static_assert(std::is_base_of_v<TResource, TDerived>, "TDerived must inherit from T");

			if (auto &&it = _path2ID.find(path); it != _path2ID.end()) [[unlikely]]
			{
				throw std::runtime_error("Resource already been loaded");
			}

			++_latestID;
			auto id = _latestID;

			std::shared_ptr<TResource> resource;
			try
			{
				resource = std::static_pointer_cast<TResource>(std::make_shared<TDerived>(std::forward<Args>(args)...));
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