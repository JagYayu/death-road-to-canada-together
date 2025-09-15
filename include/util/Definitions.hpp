/**
 * @file util/Definitions.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <cstdint>

namespace tudov
{
	// Identities

	using ChannelID = std::uint8_t;
	using ClientSessionID = std::uint64_t;
	using DelegateEventHandlerID = std::uint32_t;
	using EventID = std::uint64_t;
	using NetworkSessionSlot = std::uint32_t;
	using ResourceID = std::uint64_t;
	using ScriptID = std::uint64_t;
	using WindowID = std::uint32_t;
	using KeyboardID = std::uint32_t;
	using MouseID = std::uint32_t;

	// Resources

	using FontID = ResourceID;
	using ImageID = ResourceID;
	using TextID = ResourceID;
	using TextureID = ResourceID;
} // namespace tudov

#undef TE_DEF
