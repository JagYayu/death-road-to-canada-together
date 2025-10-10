/**
 * @file network/ReliableUDPSession.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Util/Definitions.hpp"

#include "bitsery/bitsery.h"

#include <span>

namespace tudov
{
	struct NetworkSessionData
	{
		std::span<const std::byte> bytes;
		ChannelID channelID;
	};

	enum class ENetworkSessionDataHead : std::uint8_t
	{
		/**
		 * Custom data.
		 */
		Custom = 0,
		/**
		 * Server received client's connection event, then broadcast client's metadata to all clients.
		 */
		ServerConnection,
	};

	struct NetworkSessionDataContentServerConnection
	{
		ENetworkSessionDataHead head = ENetworkSessionDataHead::ServerConnection;
		ClientSessionID clientSessionID;
	};

	template <typename TSerializer>
	void serialize(TSerializer &serializer, const NetworkSessionDataContentServerConnection &obj)
	{
		serializer.value1b(obj.head);
		serializer.value1b(obj.clientSessionID);
	}
} // namespace tudov
