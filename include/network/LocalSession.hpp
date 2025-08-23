/**
 * @file network/LocalSession.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "util/Definitions.hpp"

#include <cstddef>
#include <vector>

namespace tudov
{
	enum class ESessionEvent : std::uint8_t;

	enum class ELocalSessionSource
	{
		SendReliable,
		SendUnreliable,
		BroadcastReliable,
		BroadcastUnreliable,
	};

	struct LocalSessionMessage
	{
		ESessionEvent event;

		ELocalSessionSource source;
		std::vector<std::byte> data;
		ClientSessionToken clientToken;

		std::uint32_t clientUID;
		std::uint32_t serverUID;
	};
} // namespace tudov
