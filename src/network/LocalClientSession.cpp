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

#include <exception>
#include <memory>
#include <stdexcept>
#include <thread>
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
	_messageQueue.emplace(LocalSessionMessage::Connect(shared_from_this()), _clientSessionSlot, _localServer->GetSessionSlot());

	_localServer->ConnectClient(_clientSessionSlot, shared_from_this());

	_clientSessionState = EClientSessionState::Connected;
}

void LocalClientSession::Disconnect(EDisconnectionCode code)
{
	_clientSessionState = EClientSessionState::Disconnecting;

	_messageQueue.emplace(LocalSessionMessage::Disconnect(code, _clientSessionID), _clientSessionSlot, _localServer->GetSessionSlot());

	_clientSessionState = EClientSessionState::Disconnected;

	while (Update())
	{
	}
}

bool LocalClientSession::TryDisconnect(EDisconnectionCode code)
{
	if (GetSessionState() == EClientSessionState::Disconnected)
	{
		return false;
	}

	Disconnect(code);
	return true;
}

void LocalClientSession::SendReliable(const NetworkSessionData &data)
{
	if (_localServer == nullptr) [[unlikely]]
	{
		throw std::runtime_error("disconnected from local server");
	}

	LocalSessionMessage message{
	    .variant = LocalSessionMessage::Receive{
	        .source = ELocalSessionSource::SendReliable,
	        .bytes = std::vector<std::byte>(data.bytes.begin(), data.bytes.end()),
	        .clientID = _clientSessionID,
	    },
	    .clientSlot = _clientSessionSlot,
	    .serverSlot = _localServer->_serverSessionSlot,
	};
	_localServer->ReceiveFromClient(message);
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

		if (std::holds_alternative<LocalSessionMessage::Connect>(messageEntry.variant))
		{
			EventLocalClientConnectData data{
			    .socketType = ESocketType::Local,
			    .clientSlot = messageEntry.clientSlot,
			    .serverSlot = messageEntry.serverSlot,
			};

			coreEvents.ClientConnect().Invoke(&data, EventHandleKey(data.socketType), EEventInvocation::None);
		}
		else if (std::holds_alternative<LocalSessionMessage::Disconnect>(messageEntry.variant))
		{
			auto &event = std::get<LocalSessionMessage::Disconnect>(messageEntry.variant);

			EventLocalClientDisconnectData data{
			    .socketType = ESocketType::Local,
			    .code = event.code,
			    .clientID = event.clientID,
			    .clientSlot = messageEntry.clientSlot,
			    .serverSlot = messageEntry.serverSlot,
			};

			coreEvents.ClientDisconnect().Invoke(&data, EventHandleKey(data.socketType), EEventInvocation::None);
		}
		else if (std::holds_alternative<LocalSessionMessage::Receive>(messageEntry.variant))
		{
			UpdateReceive(messageEntry);
		}
		else
		{
		}

		_messageQueue.pop();
	}

	return updated;
}

void LocalClientSession::UpdateReceive(LocalSessionMessage &messageEntry) noexcept
{
	auto &event = std::get<LocalSessionMessage::Receive>(messageEntry.variant);

	sol::string_view received{reinterpret_cast<char *>(event.bytes.data()), event.bytes.size()};

	EventLocalClientMessageData data{
	    .socketType = ESocketType::Local,
	    .clientID = event.clientID,
	    .message = received,
	    .clientSlot = messageEntry.clientSlot,
	    .serverSlot = messageEntry.serverSlot,
	};

	if (received.size() == 2 && received[0] == '\0') [[unlikely]]
	{
		// Special case: receive client's session id
		_clientSessionID = static_cast<ClientSessionID>(received[1]);

		TE_TRACE("Received client session id {} from server", _clientSessionID);
	}
	else
	{
		GetEventManager().GetCoreEvents().ClientMessage().Invoke(&data, EventHandleKey(data.socketType), EEventInvocation::None);
	}
}

std::uint32_t LocalClientSession::GetClientSlot() noexcept
{
	return _clientSessionSlot;
}

void LocalClientSession::ReceiveFromServer(const LocalSessionMessage &message) noexcept
{
	_messageQueue.emplace(message);
}
