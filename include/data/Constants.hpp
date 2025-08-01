#pragma once

#define TE_CONSTANT static constexpr decltype(auto)

namespace tudov
{
	struct Constants
	{
		TE_CONSTANT AppName = "DR2CT";
		TE_CONSTANT AppOrganization = "Tudov";
		TE_CONSTANT DataConfigFile = "config.json";
		TE_CONSTANT DataUserDirectoryPrefix = "user_";
		TE_CONSTANT DataDeveloperAssetsDirectory = "dev";
		TE_CONSTANT DataVirtualStorageRootApp = "app";
		TE_CONSTANT DataVirtualStorageRootMods = "mods";
		TE_CONSTANT DataVirtualStorageRootUser = "user";
		TE_CONSTANT NetworkChannelsLimit = 8ui8;
		TE_CONSTANT NetworkServerMaximumClients = 4ui32;
		TE_CONSTANT NetworkServerPassword = "";
		TE_CONSTANT NetworkServerTitle = "Untitled";
		TE_CONSTANT WindowTitle = "DR2C Together";
		TE_CONSTANT WindowWidth = 1280ui32;
		TE_CONSTANT WindowHeight = 720ui32;
	};

	TE_CONSTANT AppName = Constants::AppName;
	TE_CONSTANT AppOrganization = Constants::AppOrganization;
	TE_CONSTANT AppConfigFile = Constants::DataConfigFile;
	TE_CONSTANT NetworkChannelsLimit = Constants::NetworkChannelsLimit;
	TE_CONSTANT NetworkServerMaximumClients = Constants::NetworkServerMaximumClients;
	TE_CONSTANT NetworkServerPassword = Constants::NetworkServerPassword;
	TE_CONSTANT NetworkServerTitle = Constants::NetworkServerTitle;
	TE_CONSTANT WindowTitle = Constants::WindowTitle;
	TE_CONSTANT WindowWidth = Constants::WindowWidth;
	TE_CONSTANT WindowHeight = Constants::WindowHeight;
} // namespace tudov

#undef TE_CONSTANT
