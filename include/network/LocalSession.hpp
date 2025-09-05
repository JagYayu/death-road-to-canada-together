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
#include "util/Micros.hpp"

#include <cstddef>
#include <memory>
#include <variant>
#include <vector>

namespace tudov
{
	enum class ESessionEvent : std::uint8_t;
	class LocalClientSession;
	class LocalServerSession;

	enum class ELocalSessionSource
	{
		SendReliable,
		SendUnreliable,
		BroadcastReliable,
		BroadcastUnreliable,
	};

	struct LocalSessionMessage
	{
		struct Connect
		{
			std::weak_ptr<LocalClientSession> localClient;
		};

		struct Disconnect
		{
			EDisconnectionCode code;
			ClientSessionID clientID;
		};

		struct Receive
		{
			ELocalSessionSource source;
			std::vector<std::byte> bytes;
			ClientSessionID clientID;
		};

		std::variant<Connect, Disconnect, Receive> variant;
		NetworkSessionSlot clientSlot;
		NetworkSessionSlot serverSlot;
	};

	class LocalSession
	{
		friend LocalClientSession;
		friend LocalServerSession;

		TE_STATIC_CLASS(LocalSession);

	  public:
		static void Disconnect(LocalClientSession &client, LocalServerSession &server, EDisconnectionCode code) noexcept;
	};
} // namespace tudov
