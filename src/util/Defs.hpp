#pragma once

#include <cstdint>

#define TUDOV_DEF inline constexpr decltype(auto)

namespace tudov
{
	using EventID = std::uint64_t;
	using ResourceID = std::uint64_t;
	using ScriptID = std::uint64_t;

	using ImageID = ResourceID;
	using TextureID = ResourceID;
	using FontID = ResourceID;

	TUDOV_DEF AppName = "DR2CT";
	TUDOV_DEF AppOrganization = "Tudov";
	TUDOV_DEF ConfigFile = "config.json";
	TUDOV_DEF NetworkChannelsLimit = 8ui8;
	TUDOV_DEF NetworkServerMaximumClients = 4ui32;
	TUDOV_DEF NetworkServerPassword = "";
	TUDOV_DEF NetworkServerTitle = "Untitled";
	TUDOV_DEF WindowTitle = "DR2C Together";
	TUDOV_DEF WindowWidth = 1280ui32;
	TUDOV_DEF WindowHeight = 720ui32;
} // namespace tudov

#undef TUDOV_DEF
