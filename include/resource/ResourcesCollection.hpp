#pragma once

#include "BinariesResources.hpp"
#include "FontResources.hpp"
#include "ImageResources.hpp"
#include "Resource.hpp"
#include "ResourceType.hpp"
#include "TextResources.hpp"
#include "data/Hierarchy.hpp"
#include "util/Log.hpp"

#include <filesystem>
#include <memory>
#include <vector>

namespace tudov
{
	template <typename T>
	class Resources;

	class ResourcesCollection : public ILogProvider, public IHierarchy<IResource &>
	{
	  protected:
		std::shared_ptr<BinariesResources> _binariesResources;
		std::shared_ptr<FontResources> _fontResources;
		std::shared_ptr<ImageResources> _imageResources;
		std::shared_ptr<TextResources> _textResources;
		std::vector<std::shared_ptr<Resources<IResource>>> _resourcesList;

	  public:
		static EResourceType PathExtensionToResourceType(const std::filesystem::path &path) noexcept;

		explicit ResourcesCollection() noexcept;
		~ResourcesCollection() noexcept = default;

		Log &GetLog() noexcept override;

		EHierarchyElement Check(const Path &path) noexcept override;

		bool IsData(const Path &path) noexcept override;

		bool IsDirectory(const Path &path) noexcept override;

		bool IsNone(const Path &path) noexcept override;

		IResource &Get(const Path &dataPath) override;

		EHierarchyIterationResult Foreach(const Path &directory, const IterationCallback &callback, void *callbackArgs = nullptr) override;

		BinariesResources &GetBinariesResources() noexcept;

		FontResources &GetFontResources() noexcept;

		ImageResources &GetImageResources() noexcept;

		TextResources &GetTextResources() noexcept;

		std::tuple<EResourceType, ResourceID> LoadResource(const std::filesystem::path &path, const std::vector<std::byte> &bytes);

		EResourceType UnloadResource(const std::filesystem::path &path);

		std::tuple<EResourceType, ResourceID, ResourceID> UpdateResource(const std::filesystem::path &path, const std::vector<std::byte> &bytes);
	};
} // namespace tudov
