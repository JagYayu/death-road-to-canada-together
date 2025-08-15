#pragma once

#include <cstdint>

namespace tudov
{
	// Identities

	using DelegateEventHandlerID = std::uint32_t;
	using EventID = std::uint64_t;
	using ResourceID = std::uint64_t;
	using ScriptID = std::uint64_t;

	// Resources

	using FontID = ResourceID;
	using ImageID = ResourceID;
	using TextID = ResourceID;
	using TextureID = ResourceID;
} // namespace tudov

#undef TE_DEF
