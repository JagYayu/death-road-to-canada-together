#pragma once

#include "BinariesResources.hpp"
#include "FontResources.hpp"
#include "ImageResources.hpp"
#include "ResourceType.hpp"
#include "TextResources.hpp"
#include "util/Log.hpp"

#include <filesystem>
#include <memory>
#include <vector>

namespace tudov
{
	template <typename T>
	class Resources;

	class ResourcesCollection : public ILogProvider
	{
	  protected:
		std::shared_ptr<BinariesResources> _binariesResources;
		std::shared_ptr<FontResources> _fontResources;
		std::shared_ptr<ImageResources> _imageResources;
		std::shared_ptr<TextResources> _textResources;

	  public:
		static EResourceType PathExtensionToResourceType(const std::filesystem::path &path) noexcept;

		explicit ResourcesCollection() noexcept;
		~ResourcesCollection() noexcept = default;

		Log &GetLog() noexcept override;

		BinariesResources &GetBinariesResources() noexcept;

		FontResources &GetFontResources() noexcept;

		ImageResources &GetImageResources() noexcept;

		TextResources &GetTextResources() noexcept;

		std::tuple<EResourceType, ResourceID> LoadResource(const std::filesystem::path &path, const std::vector<std::byte> &bytes);

		EResourceType UnloadResource(const std::filesystem::path &path);

		std::tuple<EResourceType, ResourceID, ResourceID> UpdateResource(const std::filesystem::path &path, const std::vector<std::byte> &bytes);
	};
} // namespace tudov
