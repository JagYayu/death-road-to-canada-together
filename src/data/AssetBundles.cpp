#include "data/AssetBundles.hpp"

#include "data/StorageManager.hpp"
#include "program/Engine.hpp"

#include "minizip/unzip.h"

using namespace tudov;

void AssetBundles::Initialize() noexcept
{
	IApplicationStorage &applicationStorage = GetStorageManager().GetApplicationStorage();

	
}

void AssetBundles::Deinitialize() noexcept
{
	//
}

const std::vector<IAssetBundles::Info> &AssetBundles::GetFounded() const noexcept
{
	return _founded;
}

const std::vector<IAssetBundles::Info> &AssetBundles::GetLoaded() const noexcept
{
	return _loaded;
}
