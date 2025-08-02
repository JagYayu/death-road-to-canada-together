#include "resource/ResourcesCollection.hpp"

#include "data/HierarchyIterationResult.hpp"
#include "resource/BinariesResources.hpp"
#include "resource/ResourceType.hpp"
#include <memory>
#include <stdexcept>

using namespace tudov;

EResourceType ResourcesCollection::PathExtensionToResourceType(const std::filesystem::path &extension) noexcept
{
	if (extension == ".ttf")
		return EResourceType::Font;
	if (extension == ".png")
		return EResourceType::Image;
	if (extension == ".txt" || extension == ".lua")
		return EResourceType::Text;
	return EResourceType::Unknown;
}

ResourcesCollection::ResourcesCollection() noexcept
    : _binariesResources(std::make_shared<BinariesResources>()),
      _fontResources(std::make_shared<FontResources>()),
      _imageResources(std::make_shared<ImageResources>()),
      _textResources(std::make_shared<TextResources>())
{
	_resourcesList = {
	    std::dynamic_pointer_cast<Resources<IResource>>(_binariesResources),
	    std::dynamic_pointer_cast<Resources<IResource>>(_fontResources),
	    std::dynamic_pointer_cast<Resources<IResource>>(_imageResources),
	    std::dynamic_pointer_cast<Resources<IResource>>(_textResources),
	};
}

Log &ResourcesCollection::GetLog() noexcept
{
	return *Log::Get("ResourcesCollection");
}

EHierarchyElement ResourcesCollection::Check(const Path &path) noexcept
{
	for (const auto &resources : _resourcesList)
	{
		EHierarchyElement result = resources->Check(path);
		if (result != EHierarchyElement::None)
		{
			return result;
		}
	}
	return EHierarchyElement::None;
}

bool ResourcesCollection::IsData(const Path &path) noexcept
{
	for (const auto &resources : _resourcesList)
	{
		if (resources->IsData(path))
		{
			return true;
		}
	}
	return false;
}

bool ResourcesCollection::IsDirectory(const Path &path) noexcept
{
	for (const auto &resources : _resourcesList)
	{
		if (resources->IsDirectory(path))
		{
			return true;
		}
	}
	return false;
}

bool ResourcesCollection::IsNone(const Path &path) noexcept
{
	for (const auto &resources : _resourcesList)
	{
		if (resources->IsNone(path))
		{
			return true;
		}
	}
	return false;
}

IResource &ResourcesCollection::Get(const Path &dataPath)
{
	for (const auto &resources : _resourcesList)
	{
		if (resources->IsData(dataPath))
		{
			return resources->Get(dataPath);
		}
	}
	throw std::runtime_error("resource not found");
}

EHierarchyIterationResult ResourcesCollection::Foreach(const Path &directory, const IterationCallback &callback, void *callbackArgs)
{
	for (const auto &resources : _resourcesList)
	{
		EHierarchyIterationResult result = resources->Foreach(directory, callback);
		if (result != EHierarchyIterationResult::Continue)
		{
			return result;
		}
	}
	return EHierarchyIterationResult::Continue;
}

BinariesResources &ResourcesCollection::GetBinariesResources() noexcept
{
	return *_binariesResources;
}

FontResources &ResourcesCollection::GetFontResources() noexcept
{
	return *_fontResources;
}

ImageResources &ResourcesCollection::GetImageResources() noexcept
{
	return *_imageResources;
}

TextResources &ResourcesCollection::GetTextResources() noexcept
{
	return *_textResources;
}

std::tuple<EResourceType, ResourceID> ResourcesCollection::LoadResource(const std::filesystem::path &path, const std::vector<std::byte> &bytes)
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
		// _fontResources->Load(path.generic_string(), bytes);
		break;
	case EResourceType::Binary:
	default:
		id = _binariesResources->Load(path.generic_string(), bytes);
		break;
	}

	return {type, id};
}

inline EResourceType ResourcesCollection::UnloadResource(const std::filesystem::path &path)
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
		break;
	case EResourceType::Image:
		_imageResources->Unload(path.generic_string());
		break;
	case EResourceType::Audio:
		break;
	case EResourceType::Font:
		break;
	case EResourceType::Binary:
		_binariesResources->Unload(path.generic_string());
		break;
	default:
		break;
	}

	return type;
}

std::tuple<EResourceType, ResourceID, ResourceID> ResourcesCollection::UpdateResource(const std::filesystem::path &path, const std::vector<std::byte> &bytes)
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
	case EResourceType::Binary:
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
