/**
 * @file network/LocalClient.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "network/LocalClientSession.hpp"

#include "event/CoreEvents.hpp"
#include "event/CoreEventsData.hpp"
#include "event/EventInvocation.hpp"
#include "event/EventManager.hpp"
#include "event/RuntimeEvent.hpp"
#include "network/ClientSessionState.hpp"
#include "network/LocalClientSession.hpp"
#include "network/LocalServerSession.hpp"
#include "network/LocalSession.hpp"
#include "network/NetworkManager.hpp"
#include "network/NetworkSessionData.hpp"
#include "network/SocketType.hpp"
#include "system/LogMicros.hpp"
#include "util/Micros.hpp"

#include <memory>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <vector>

using namespace tudov;

LocalClientSession::LocalClientSession(INetworkManager &network, NetworkSessionSlot clientSlot) noexcept
    : _networkManager(network),
      _clientSessionState(EClientSessionState::Disconnected),
      _clientSessionSlot(clientSlot),
      _localServer(nullptr)
{
}

INetworkManager &LocalClientSession::GetNetworkManager() noexcept
{
	return _networkManager;
}

ESocketType LocalClientSession::GetSocketType() const noexcept
{
	return ESocketType::Local;
}

NetworkSessionSlot LocalClientSession::GetSessionSlot() noexcept
{
	return _clientSessionSlot;
}

Log &LocalClientSession::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(LocalClientSession));
}

ClientSessionID LocalClientSession::GetSessionID() const noexcept
{
	return _clientSessionID;
}

EClientSessionState LocalClientSession::GetSessionState() const noexcept
{
	return _clientSessionState;
}

void LocalClientSession::Connect(const IClientSession::ConnectArgs &baseArgs)
{
	_clientSessionState = EClientSessionState::Connecting;

	const ConnectArgs &args = dynamic_cast<const ConnectArgs &>(baseArgs);

	_localServer = args.localServer;

	if (_localServer == nullptr) [[unlikely]]
	{
		throw std::runtime_error("No local server provided");
	}

	_localServer->AddClient(_clientSessionSlot, shared_from_this());

	_clientSessionState = EClientSessionState::Connected;
}

void LocalClientSession::Disconnect()
{
	_clientSessionState = EClientSessionState::Disconnecting;
	_localServer = nullptr;
	_clientSessionState = EClientSessionState::Disconnected;
}

bool LocalClientSession::TryDisconnect()
{
	if (GetSessionState() == EClientSessionState::Disconnected)
	{
		return false;
	}

	Disconnect();
	return true;
}

void LocalClientSession::SendReliable(const NetworkSessionData &data)
{
	if (_localServer == nullptr) [[unlikely]]
	{
		throw std::runtime_error("disconnected from local server");
	}

	LocalSessionMessage message{
	    .event = ESessionEvent::Receive,
	    .source = ELocalSessionSource::SendReliable,
	    .bytes = std::vector<std::byte>(data.bytes.begin(), data.bytes.end()),
	    .clientID = _clientSessionID,
	    .clientSlot = _clientSessionSlot,
	    .serverSlot = _localServer->_serverSessionSlot,
	};
	_localServer->Receive(message);
}

void LocalClientSession::SendUnreliable(const NetworkSessionData &data)
{
	SendReliable(data);
}

bool LocalClientSession::Update()
{
	bool updated = false;

	ICoreEvents &coreEvents = GetEventManager().GetCoreEvents();

	while (!_messageQueue.empty())
	{
		updated = true;

		auto &messageEntry = _messageQueue.front();
		switch (messageEntry.event)
		{
		case ESessionEvent::Connect:
		{
			EventLocalClientConnectData data{
			    .socketType = ESocketType::Local,
			    .clientSlot = messageEntry.clientSlot,
			    .serverSlot = messageEntry.serverSlot,
			};

			coreEvents.ClientConnect().Invoke(&data, data.socketType, EEventInvocation::None);

			break;
		}
		case ESessionEvent::Receive:
		{
			sol::string_view message{reinterpret_cast<char *>(messageEntry.bytes.data()), messageEntry.bytes.size()};

			EventLocalClientMessageData data{
			    .socketType = ESocketType::Local,
			    .clientID = messageEntry.clientID,
			    .message = message,
			    .clientSlot = messageEntry.clientSlot,
			    .serverSlot = messageEntry.serverSlot,
			};

			coreEvents.ClientMessage().Invoke(&data, data.socketType, EEventInvocation::None);

			break;
		}
		case ESessionEvent::Disconnect:
		{
			EventLocalClientDisconnectData data{
			    .socketType = ESocketType::Local,
			    .clientID = messageEntry.clientID,
			    .clientSlot = messageEntry.clientSlot,
			    .serverSlot = messageEntry.serverSlot,
			};

			coreEvents.ClientDisconnect().Invoke(&data, data.socketType, EEventInvocation::None);

			break;
		}
		default:
			TE_WARN("Unknown session event: {}", static_cast<std::underlying_type_t<ESessionEvent>>(messageEntry.event));
			break;
		}
	}

	return updated;
}

std::uint32_t LocalClientSession::GetClientSlot() noexcept
{
	return _clientSessionSlot;
}

void LocalClientSession::Receive(const LocalSessionMessage &message) noexcept
{
	_messageQueue.emplace(message);
}
