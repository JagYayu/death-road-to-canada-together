/**
 * @file resource/GlobalResourcesCollection.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "AudioResources.hpp"
#include "BinariesResources.hpp"
#include "FontResources.hpp"
#include "ImageResources.hpp"
#include "Resource.hpp"
#include "ResourceType.hpp"
#include "TextResources.hpp"
#include "Program/EngineComponent.hpp"
#include "System/Log.hpp"

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
	class GlobalResourcesCollection final : public IEngineComponent, private ILogProvider
	{
	  protected:
		Context &_context;

		std::shared_ptr<AudioResources> _audioResources;
		std::shared_ptr<BinariesResources> _binariesResources;
		std::shared_ptr<FontResources> _fontResources;
		std::shared_ptr<ImageResources> _imageResources;
		std::shared_ptr<TextResources> _textResources;
		std::vector<std::shared_ptr<Resources<IResource>>> _resourcesList;

		DelegateEventHandlerID _handlerIDOnVFSMountFile = 0;
		DelegateEventHandlerID _handlerIDOnVFSDismountFile = 0;
		DelegateEventHandlerID _handlerIDOnVFSRemountFile = 0;

	  public:
		static EResourceType PathExtensionToResourceType(const std::filesystem::path &path) noexcept;

		explicit GlobalResourcesCollection(Context &context) noexcept;
		explicit GlobalResourcesCollection(const GlobalResourcesCollection &) noexcept = delete;
		explicit GlobalResourcesCollection(GlobalResourcesCollection &&) noexcept = delete;
		GlobalResourcesCollection &operator=(const GlobalResourcesCollection &) noexcept = delete;
		GlobalResourcesCollection &operator=(GlobalResourcesCollection &&) noexcept = delete;
		~GlobalResourcesCollection() noexcept = default;

		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		void PreInitialize() noexcept override;
		void Initialize() noexcept override;
		void Deinitialize() noexcept override;
		void PostDeinitialize() noexcept override;

		BinariesResources &GetBinariesResources() noexcept;

		FontResources &GetFontResources() noexcept;

		ImageResources &GetImageResources() noexcept;

		TextResources &GetTextResources() noexcept;

		std::tuple<EResourceType, ResourceID> LoadResource(const std::filesystem::path &path, const std::vector<std::byte> &bytes);

		EResourceType UnloadResource(const std::filesystem::path &path);

		std::tuple<EResourceType, ResourceID, ResourceID> ReloadResource(const std::filesystem::path &path, const std::vector<std::byte> &bytes);

		const BinariesResources &GetBinariesResources() const noexcept
		{
			return const_cast<GlobalResourcesCollection *>(this)->GetBinariesResources();
		}

		const FontResources &GetFontResources() const noexcept
		{
			return const_cast<GlobalResourcesCollection *>(this)->GetFontResources();
		}

		const ImageResources &GetImageResources() const noexcept
		{
			return const_cast<GlobalResourcesCollection *>(this)->GetImageResources();
		}

		const TextResources &GetTextResources() const noexcept
		{
			return const_cast<GlobalResourcesCollection *>(this)->GetTextResources();
		}
	};
} // namespace tudov
