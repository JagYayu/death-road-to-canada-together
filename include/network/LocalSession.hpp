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

#include "DisconnectionCode.hpp"
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
		struct Variant
		{
			// SessionEvent is Receive
			std::vector<std::byte> bytes;
			// SessionEvent is Disconnect
			EDisconnectionCode code;
		};

		ESessionEvent event;

		ELocalSessionSource source;
		Variant variant;
		ClientSessionID clientID;

		NetworkSessionSlot clientSlot;
		NetworkSessionSlot serverSlot;
	};
} // namespace tudov
