#include "resource/GlobalResourcesCollection.hpp"

#include "data/VirtualFileSystem.hpp"
#include "resource/BinariesResources.hpp"
#include "resource/ResourceType.hpp"

#include <memory>

using namespace tudov;

EResourceType GlobalResourcesCollection::PathExtensionToResourceType(const std::filesystem::path &extension) noexcept
{
	if (extension == ".ttf")
		return EResourceType::Font;
	if (extension == ".png")
		return EResourceType::Image;
	if (extension == ".txt" || extension == ".lua")
		return EResourceType::Text;
	return EResourceType::Unknown;
}

GlobalResourcesCollection::GlobalResourcesCollection(Context &context) noexcept
    : _context(context),
      _binariesResources(std::make_shared<BinariesResources>()),
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

Context &GlobalResourcesCollection::GetContext() noexcept
{
	return _context;
}

Log &GlobalResourcesCollection::GetLog() noexcept
{
	return *Log::Get("GlobalResourcesCollection");
}

void GlobalResourcesCollection::Initialize() noexcept
{
	auto &vfs = GetVirtualFileSystem();

	_handlerIDOnVFSMountFile = vfs.GetOnMountFile() += [this](const std::filesystem::path &path, const std::vector<std::byte> &bytes)
	{
		LoadResource(path, bytes);
	};
	_handlerIDOnVFSDismountFile = vfs.GetOnDismountFile() += [this](const std::filesystem::path &path)
	{
		UnloadResource(path);
	};
	_handlerIDOnVFSRemountFile = vfs.GetOnRemountFile() += [this](const std::filesystem::path &path, const std::vector<std::byte> &bytes, const std::vector<std::byte> &)
	{
		ReloadResource(path, bytes);
	};
}

void GlobalResourcesCollection::Deinitialize() noexcept
{
	auto &vfs = GetVirtualFileSystem();

	vfs.GetOnMountFile() -= _handlerIDOnVFSMountFile;
	vfs.GetOnDismountFile() -= _handlerIDOnVFSDismountFile;
	vfs.GetOnRemountFile() -= _handlerIDOnVFSRemountFile;
}

// EHierarchyElement GlobalResourcesCollection::Check(const Path &path) noexcept
// {
// 	for (const auto &resources : _resourcesList)
// 	{
// 		EHierarchyElement result = resources->Check(path);
// 		if (result != EHierarchyElement::None)
// 		{
// 			return result;
// 		}
// 	}
// 	return EHierarchyElement::None;
// }

// bool GlobalResourcesCollection::IsData(const Path &path) noexcept
// {
// 	for (const auto &resources : _resourcesList)
// 	{
// 		if (resources->IsData(path))
// 		{
// 			return true;
// 		}
// 	}
// 	return false;
// }

// bool GlobalResourcesCollection::IsDirectory(const Path &path) noexcept
// {
// 	for (const auto &resources : _resourcesList)
// 	{
// 		if (resources->IsDirectory(path))
// 		{
// 			return true;
// 		}
// 	}
// 	return false;
// }

// bool GlobalResourcesCollection::IsNone(const Path &path) noexcept
// {
// 	for (const auto &resources : _resourcesList)
// 	{
// 		if (resources->IsNone(path))
// 		{
// 			return true;
// 		}
// 	}
// 	return false;
// }

// IResource &GlobalResourcesCollection::Get(const Path &dataPath)
// {
// 	for (const auto &resources : _resourcesList)
// 	{
// 		if (resources->IsData(dataPath))
// 		{
// 			return resources->Get(dataPath);
// 		}
// 	}
// 	throw std::runtime_error("resource not found");
// }

// EHierarchyIterationResult GlobalResourcesCollection::Foreach(const Path &directory, const IterationCallback &callback, void *callbackArgs)
// {
// 	for (const auto &resources : _resourcesList)
// 	{
// 		EHierarchyIterationResult result = resources->Foreach(directory, callback);
// 		if (result != EHierarchyIterationResult::Continue)
// 		{
// 			return result;
// 		}
// 	}
// 	return EHierarchyIterationResult::Continue;
// }

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
		// _fontResources->Load(path.generic_string(), bytes);
		break;
	case EResourceType::Binary:
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
