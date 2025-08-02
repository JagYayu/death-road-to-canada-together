#pragma once

#include "BinariesResources.hpp"
#include "FontResources.hpp"
#include "ImageResources.hpp"
#include "Resource.hpp"
#include "ResourceType.hpp"
#include "TextResources.hpp"
#include "event/DelegateEvent.hpp"
#include "program/EngineComponent.hpp"
#include "util/Log.hpp"

#include <filesystem>
#include <memory>
#include <vector>

namespace tudov
{
	template <typename T>
	class Resources;

	/**
	 * Manage all kinds of game resources.
	 * Automatically monitor VFS, load/unload/update resources when VFS has any event triggered.
	 */
	class ResourcesCollection : public IEngineComponent, public ILogProvider
	{
	  protected:
		Context &_context;

		std::shared_ptr<BinariesResources> _binariesResources;
		std::shared_ptr<FontResources> _fontResources;
		std::shared_ptr<ImageResources> _imageResources;
		std::shared_ptr<TextResources> _textResources;
		std::vector<std::shared_ptr<Resources<IResource>>> _resourcesList;

		DelegateEvent<const std::filesystem::path &, const std::vector<std::byte> &>::HandlerID _handlerIDOnVFSMountFile;
		DelegateEvent<const std::filesystem::path &>::HandlerID _handlerIDOnVFSDismountFile;
		DelegateEvent<const std::filesystem::path &, const std::vector<std::byte> &, const std::vector<std::byte> &>::HandlerID _handlerIDOnVFSRemountFile;

	  public:
		static EResourceType PathExtensionToResourceType(const std::filesystem::path &path) noexcept;

		explicit ResourcesCollection(Context &context) noexcept;
		~ResourcesCollection() noexcept = default;

		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		void Initialize() noexcept override;
		void Deinitialize() noexcept override;

		BinariesResources &GetBinariesResources() noexcept;

		FontResources &GetFontResources() noexcept;

		ImageResources &GetImageResources() noexcept;

		TextResources &GetTextResources() noexcept;

		std::tuple<EResourceType, ResourceID> LoadResource(const std::filesystem::path &path, const std::vector<std::byte> &bytes);

		EResourceType UnloadResource(const std::filesystem::path &path);

		std::tuple<EResourceType, ResourceID, ResourceID> ReloadResource(const std::filesystem::path &path, const std::vector<std::byte> &bytes);
	};
} // namespace tudov
