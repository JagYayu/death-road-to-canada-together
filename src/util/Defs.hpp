#pragma once

#include <cstdint>
#include <string_view>

namespace tudov
{
	using EventID = std::uint64_t;
	using ResourceID = std::uint64_t;
	using ScriptID = std::uint64_t;

	using ImageID = ResourceID;
	using TextureID = ResourceID;
	using FontID = ResourceID;

	constexpr std::uint8_t NetworkChannelsLimit = 8;
	constexpr std::uint32_t NetworkServerMaximumClients = 4;
	constexpr std::string_view NetworkServerPassword = "";
	constexpr std::string_view NetworkServerTitle = "Untitled";
} // namespace tudov