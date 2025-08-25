/**
 * @file network/LocalServerSession.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "network/LocalServerSession.hpp"

#include "event/CoreEvents.hpp"
#include "event/CoreEventsData.hpp"
#include "event/EventInvocation.hpp"
#include "event/EventManager.hpp"
#include "event/RuntimeEvent.hpp"
#include "network/DisconnectionCode.hpp"
#include "network/LocalClientSession.hpp"
#include "network/LocalServerSession.hpp"
#include "network/LocalSession.hpp"
#include "network/NetworkManager.hpp"
#include "network/NetworkSessionData.hpp"
#include "network/ServerSessionState.hpp"
#include "network/SocketType.hpp"
#include "system/LogMicros.hpp"
#include "util/Micros.hpp"

#include <optional>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

using namespace tudov;

// LocalServerSession::Message::Message(bool reliable, std::span<const std::byte> data) noexcept
// {
// 	this->source = reliable ? BroadcastReliable : BroadcastUnreliable;
// 	this->data = std::vector<std::byte>(data.begin(), data.end());
// }

// LocalServerSession::Message::Message(bool reliable, ClientSessionID clientID, std::span<const std::byte> data) noexcept
// {
// 	this->source = reliable ? SendReliable : SendUnreliable;
// 	this->clientSessionID = clientID;
// 	this->data = std::vector<std::byte>(data.begin(), data.end());
// }

LocalServerSession::LocalServerSession(INetworkManager &network, NetworkSessionSlot serverSlot) noexcept
    : _networkManager(network),
      _serverSessionSlot(serverSlot),
      _sessionState(EServerSessionState::Shutdown),
      _latestID(0)
{
}

INetworkManager &LocalServerSession::GetNetworkManager() noexcept
{
	return _networkManager;
}

ESocketType LocalServerSession::GetSocketType() const noexcept
{
	return ESocketType::Local;
}

NetworkSessionSlot LocalServerSession::GetSessionSlot() noexcept
{
	return _serverSessionSlot;
}

Log &LocalServerSession::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(LocalServerSession));
}

EServerSessionState LocalServerSession::GetSessionState() const noexcept
{
	return _sessionState;
}

void LocalServerSession::Host(const HostArgs &args)
{
	_sessionState = EServerSessionState::Starting;

	_hostInfo = std::make_unique<HostInfo>(HostInfo{
	    .title = args.title,
	    .password = args.password,
	    .maximumClients = args.maximumClients,
	});

	_sessionState = EServerSessionState::Hosting;
}

void LocalServerSession::Shutdown()
{
	_sessionState = EServerSessionState::Stopping;
	_hostInfo = nullptr;
	_sessionState = EServerSessionState::Shutdown;
}

bool LocalServerSession::TryShutdown()
{
	if (_sessionState == EServerSessionState::Hosting)
	{
		Shutdown();
		return true;
	}
	return false;
}

std::optional<std::string_view> LocalServerSession::GetTitle() const noexcept
{
	return _hostInfo != nullptr ? std::make_optional(_hostInfo->title) : std::nullopt;
}

std::optional<std::string_view> LocalServerSession::GetPassword() const noexcept
{
	return _hostInfo != nullptr ? std::make_optional(_hostInfo->password) : std::nullopt;
}

std::optional<std::size_t> LocalServerSession::GetMaxClients() const noexcept
{
	return _hostInfo != nullptr ? std::make_optional(_hostInfo->maximumClients) : std::nullopt;
}

void LocalServerSession::SendReliable(ClientSessionID clientSessionID, const NetworkSessionData &data)
{
	EnqueueMessage(clientSessionID, data, ELocalSessionSource::SendReliable);
}

void LocalServerSession::SendUnreliable(ClientSessionID clientSessionID, const NetworkSessionData &data)
{
	EnqueueMessage(clientSessionID, data, ELocalSessionSource::SendUnreliable);
}

void LocalServerSession::BroadcastReliable(const NetworkSessionData &data)
{
	EnqueueMessage(0, data, ELocalSessionSource::BroadcastReliable);
}

void LocalServerSession::BroadcastUnreliable(const NetworkSessionData &data)
{
	EnqueueMessage(0, data, ELocalSessionSource::BroadcastUnreliable);
}

void LocalServerSession::EnqueueMessage(ClientSessionID clientSessionID, const NetworkSessionData &data, ELocalSessionSource source)
{
	auto it = _hostInfo->localClients.find(clientSessionID);
	if (it == _hostInfo->localClients.end() && !it->second.expired())
	{
		throw std::runtime_error("client haven't connect");
	}

	LocalSessionMessage message{
	    .event = ESessionEvent::Receive,
	    .source = ELocalSessionSource::SendReliable,
	    .bytes = std::vector<std::byte>(data.bytes.begin(), data.bytes.end()),
	    .clientID = it->second.lock()->GetSessionID(),
	    .clientSlot = it->second.lock()->GetClientSlot(),
	    .serverSlot = _serverSessionSlot,
	};
	_messageQueue.emplace(message);
}

ClientSessionID LocalServerSession::ClientSlotToClientID(NetworkSessionSlot clientSlot) const noexcept
{
	return static_cast<ClientSessionID>(clientSlot) + 1;
}

bool LocalServerSession::Update()
{
	bool updated = false;

	std::unordered_map<ClientSessionID, std::weak_ptr<LocalClientSession>> &localClients = _hostInfo->localClients;
	for (auto it = localClients.begin(); it != localClients.end();)
	{
		if (it->second.expired()) [[unlikely]]
		{
			it = localClients.erase(it);
		}
		else [[likely]]
		{
			++it;
		}
	}

	ICoreEvents &coreEvents = GetEventManager().GetCoreEvents();

	while (!_messageQueue.empty())
	{
		LocalSessionMessage &messageEntry = _messageQueue.front();

		switch (messageEntry.event)
		{
		case ESessionEvent::Connect:
		{
			EventLocalServerConnectData data{
			    .socketType = ESocketType::Local,
			    .clientSlot = messageEntry.clientSlot,
			    .serverSlot = messageEntry.serverSlot,
			};

			TE_TRACE("Connect event, client", data.clientSlot);

			coreEvents.ServerConnect().Invoke(&data, data.socketType, EEventInvocation::None);

			ClientSessionID id = NewID();
			std::shared_ptr<LocalClientSession> client = _hostInfo->localClients[messageEntry.clientID].lock();
			TE_ASSERT(client != nullptr);

			// TODO its not done yet
			// std::span<std::byte> broadcastData{reinterpret_cast<std::byte *>(), event.packet->dataLength};
			// BroadcastReliable(broadcastData, event.channelID);

			break;
		}
		[[unlikely]] default:
			break;
		}

		updated = true;

		_messageQueue.pop();
	}

	return updated;
}

void LocalServerSession::Receive(const LocalSessionMessage &message) noexcept
{
	auto it = _hostInfo->localClients.find(message.clientID);
	if (it == _hostInfo->localClients.end() || it->second.expired()) [[unlikely]]
	{
		return;
	}

	it->second.lock()->Receive(message);
}

void LocalServerSession::AddClient(NetworkSessionSlot clientSlot, std::weak_ptr<LocalClientSession> localClient)
{
	ClientSessionID clientID = ClientSlotToClientID(clientSlot);
	if (clientID == 0) [[unlikely]]
	{
		throw std::runtime_error("Invalid client uid");
	}

	_hostInfo->localClients[clientID] = localClient;
}

ClientSessionID LocalServerSession::NewID() noexcept
{
	++_latestID;
	return _latestID;
}
