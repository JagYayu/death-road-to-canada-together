#pragma once

#include <cstdint>

namespace tudov
{
	// Identities

	using EventID = std::uint64_t;
	using ResourceID = std::uint64_t;
	using ScriptID = std::uint64_t;

	// Resources

	using ImageID = ResourceID;
	using TextureID = ResourceID;
	using FontID = ResourceID;
} // namespace tudov

#undef TE_DEF
