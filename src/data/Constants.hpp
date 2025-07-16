#pragma once

#define TE_CONSTANT static constexpr decltype(auto)

namespace tudov
{
	TE_CONSTANT AppName = "DR2CT";
	TE_CONSTANT AppOrganization = "Tudov";
	TE_CONSTANT AppConfigFile = "config.json";
	TE_CONSTANT NetworkChannelsLimit = 8ui8;
	TE_CONSTANT NetworkServerMaximumClients = 4ui32;
	TE_CONSTANT NetworkServerPassword = "";
	TE_CONSTANT NetworkServerTitle = "Untitled";
	TE_CONSTANT WindowTitle = "DR2C Together";
	TE_CONSTANT WindowWidth = 1280ui32;
	TE_CONSTANT WindowHeight = 720ui32;
} // namespace tudov

#undef TE_CONSTANT
