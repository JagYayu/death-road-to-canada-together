/**
 * @file resource/GlobalResourcesCollection.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Resource/GlobalResourcesCollection.hpp"

#include "Data/VirtualFileSystem.hpp"
#include "Resource/AudioResources.hpp"
#include "Resource/BinariesResources.hpp"
#include "Resource/ResourceType.hpp"

#include <memory>

using namespace tudov;

static constexpr std::array extensionsFont = {
    ".ttf",
};

static constexpr std::array extensionsImage = {
    ".png",
};

static constexpr std::array extensionsAudio = {
    ".mp3",
};

static constexpr std::array extensionsText = {
    ".txt",
    ".lua",
    ".json",
};

static constexpr std::array extensionsBinaries = {
    ".zip",
};

EResourceType GlobalResourcesCollection::PathExtensionToResourceType(const std::filesystem::path &extension) noexcept
{
	if (std::ranges::find(extensionsFont, extension) != extensionsFont.end())
		return EResourceType::Font;
	if (std::ranges::find(extensionsImage, extension) != extensionsImage.end())
		return EResourceType::Image;
	if (std::ranges::find(extensionsAudio, extension) != extensionsAudio.end())
		return EResourceType::Audio;
	if (std::ranges::find(extensionsText, extension) != extensionsText.end())
		return EResourceType::Text;
	if (std::ranges::find(extensionsBinaries, extension) != extensionsBinaries.end())
		return EResourceType::Binaries;
	return EResourceType::Unknown;
}

GlobalResourcesCollection::GlobalResourcesCollection(Context &context) noexcept
    : _context(context)
{
	_audioResources = std::make_shared<AudioResources>();
	_binariesResources = std::make_shared<BinariesResources>();
	_fontResources = std::make_shared<FontResources>();
	_imageResources = std::make_shared<ImageResources>();
	_textResources = std::make_shared<TextResources>();

	using Base = Resources<IResource>;

	_resourcesList = {
	    std::dynamic_pointer_cast<Base>(_binariesResources),
	    std::dynamic_pointer_cast<Base>(_textResources),
	    std::dynamic_pointer_cast<Base>(_fontResources),
	    std::dynamic_pointer_cast<Base>(_imageResources),
	};
}

Context &GlobalResourcesCollection::GetContext() noexcept
{
	return _context;
}

Log &GlobalResourcesCollection::GetLog() noexcept
{
	return *Log::Get("GlobalResourcesCollection");
}

void GlobalResourcesCollection::PreInitialize() noexcept
{
	auto &virtualFileSystem = GetVirtualFileSystem();

	_handlerIDOnVFSMountFile = virtualFileSystem.GetOnMountFile() += [this](const std::filesystem::path &path, const std::vector<std::byte> &bytes, EResourceType &resourceType)
	{
		auto [type, _] = LoadResource(path, bytes);
		resourceType = type;
	};
	_handlerIDOnVFSDismountFile = virtualFileSystem.GetOnDismountFile() += [this](const std::filesystem::path &path)
	{
		UnloadResource(path);
	};
	_handlerIDOnVFSRemountFile = virtualFileSystem.GetOnRemountFile() += [this](const std::filesystem::path &path, const std::vector<std::byte> &bytes, const std::vector<std::byte> &, EResourceType &resourceType)
	{
		auto [type, _, _1] = ReloadResource(path, bytes);
		resourceType = type;
	};
}

void GlobalResourcesCollection::Initialize() noexcept
{
}

void GlobalResourcesCollection::Deinitialize() noexcept
{
}

void GlobalResourcesCollection::PostDeinitialize() noexcept
{
	auto &virtualFileSystem = GetVirtualFileSystem();

	virtualFileSystem.GetOnMountFile() -= _handlerIDOnVFSMountFile;
	virtualFileSystem.GetOnDismountFile() -= _handlerIDOnVFSDismountFile;
	virtualFileSystem.GetOnRemountFile() -= _handlerIDOnVFSRemountFile;

	_handlerIDOnVFSMountFile = 0;
	_handlerIDOnVFSDismountFile = 0;
	_handlerIDOnVFSRemountFile = 0;
}

BinariesResources &GlobalResourcesCollection::GetBinariesResources() noexcept
{
	return *_binariesResources;
}

FontResources &GlobalResourcesCollection::GetFontResources() noexcept
{
	return *_fontResources;
}

ImageResources &GlobalResourcesCollection::GetImageResources() noexcept
{
	return *_imageResources;
}

TextResources &GlobalResourcesCollection::GetTextResources() noexcept
{
	return *_textResources;
}

std::tuple<EResourceType, ResourceID> GlobalResourcesCollection::LoadResource(const std::filesystem::path &path, const std::vector<std::byte> &bytes)
{
	if (!path.has_extension()) [[unlikely]]
	{
		Warn("Path must point to a file with extension: {}", path.generic_string());
		return {EResourceType::Unknown, 0};
	}

	EResourceType type = PathExtensionToResourceType(path.extension());
	ResourceID id = 0;

	switch (type)
	{
	case EResourceType::Text:
		id = _textResources->Load(path.generic_string(), bytes);
		break;
	case EResourceType::Image:
		id = _imageResources->Load(path.generic_string(), bytes);
		break;
	case EResourceType::Audio:
		break;
	case EResourceType::Font:
		id = _fontResources->Load(path.generic_string(), bytes);
		break;
	case EResourceType::Binaries:
	default:
		id = _binariesResources->Load(path.generic_string(), bytes);
		break;
	}

	return {type, id};
}

inline EResourceType GlobalResourcesCollection::UnloadResource(const std::filesystem::path &path)
{
	if (!path.has_extension()) [[unlikely]]
	{
		Warn("Path must point to a file with extension: {}", path.generic_string());
		return EResourceType::Unknown;
	}

	EResourceType type = PathExtensionToResourceType(path.extension());

	switch (type)
	{
	case EResourceType::Text:
		_textResources->Unload(path.generic_string());
		break;
	case EResourceType::Image:
		_imageResources->Unload(path.generic_string());
		break;
	case EResourceType::Audio:
		break;
	case EResourceType::Font:
		break;
	case EResourceType::Binaries:
		_binariesResources->Unload(path.generic_string());
		break;
	default:
		break;
	}

	return type;
}

std::tuple<EResourceType, ResourceID, ResourceID> GlobalResourcesCollection::ReloadResource(const std::filesystem::path &path, const std::vector<std::byte> &bytes)
{
	if (!path.has_extension()) [[unlikely]]
	{
		Warn("Path must point to a file with extension: {}", path.generic_string());
		return {EResourceType::Unknown, 0, 0};
	}

	std::string pathStr = path.generic_string();
	EResourceType type = PathExtensionToResourceType(path.extension());
	ResourceID oldID = 0;
	ResourceID newID = 0;

	switch (type)
	{
	case EResourceType::Text:
		oldID = _textResources->GetResourceID(pathStr);
		if (oldID != 0)
		{
			_textResources->Unload(oldID);
		}
		newID = _textResources->Load(pathStr, bytes);
		break;
	case EResourceType::Image:
		oldID = _imageResources->GetResourceID(pathStr);
		if (oldID != 0)
		{
			_imageResources->Unload(oldID);
		}
		newID = _imageResources->Load(pathStr, bytes);
		break;
	case EResourceType::Audio:
		break;
	case EResourceType::Font:
		break;
	case EResourceType::Binaries:
		oldID = _binariesResources->GetResourceID(pathStr);
		if (oldID != 0)
		{
			_binariesResources->Unload(oldID);
		}
		newID = _binariesResources->Load(pathStr, bytes);
		break;
	default:
		break;
	}

	return {type, oldID, newID};
}
