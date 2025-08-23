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

// LocalServerSession::Message::Message(bool reliable, ClientSessionToken clientID, std::span<const std::byte> data) noexcept
// {
// 	this->source = reliable ? SendReliable : SendUnreliable;
// 	this->clientSessionToken = clientID;
// 	this->data = std::vector<std::byte>(data.begin(), data.end());
// }

LocalServerSession::LocalServerSession(INetworkManager &network, std::uint32_t serverUID) noexcept
    : _networkManager(network),
      _serverUID(serverUID),
      _sessionState(EServerSessionState::Shutdown),
      _latestToken(0)
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

void LocalServerSession::SendReliable(ClientSessionToken clientSessionToken, std::span<const std::byte> data, ChannelID channelID)
{
	EnqueueMessage(clientSessionToken, data, channelID, ELocalSessionSource::SendReliable);
}

void LocalServerSession::SendUnreliable(ClientSessionToken clientSessionToken, std::span<const std::byte> data, ChannelID channelID)
{
	EnqueueMessage(clientSessionToken, data, channelID, ELocalSessionSource::SendUnreliable);
}

void LocalServerSession::BroadcastReliable(std::span<const std::byte> data, ChannelID channelID)
{
	EnqueueMessage(0, data, channelID, ELocalSessionSource::BroadcastReliable);
}

void LocalServerSession::BroadcastUnreliable(std::span<const std::byte> data, ChannelID channelID)
{
	EnqueueMessage(0, data, channelID, ELocalSessionSource::BroadcastUnreliable);
}

void LocalServerSession::EnqueueMessage(ClientSessionToken clientSessionToken, std::span<const std::byte> data, ChannelID channelID, ELocalSessionSource source)
{
	auto it = _hostInfo->localClients.find(clientSessionToken);
	if (it == _hostInfo->localClients.end() && !it->second.expired())
	{
		throw std::runtime_error("client haven't connect");
	}

	LocalSessionMessage message{
	    .event = ESessionEvent::Receive,
	    .source = ELocalSessionSource::SendReliable,
	    .data = std::vector<std::byte>(data.begin(), data.end()),
	    .clientToken = it->second.lock()->GetToken(),
	    .clientUID = it->second.lock()->GetClientUID(),
	    .serverUID = _serverUID,
	};
	_messageQueue.emplace(message);
}

ClientSessionToken LocalServerSession::ClientUIDToClientID(std::uint32_t clientUID) const noexcept
{
	return static_cast<ClientSessionToken>(clientUID) + 1;
}

bool LocalServerSession::Update()
{
	bool updated = false;

	std::unordered_map<ClientSessionToken, std::weak_ptr<LocalClientSession>> &localClients = _hostInfo->localClients;
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
			    .clientUID = messageEntry.clientUID,
			    .serverUID = messageEntry.serverUID,
			    .disconnect = EDisconnectionCode::None,
			};

			TE_TRACE("Connect event, client", data.clientUID);

			coreEvents.ServerConnect().Invoke(&data, data.socketType, EEventInvocation::None);

			if (data.disconnect != EDisconnectionCode::None)
			{
				TE_ASSERT(_hostInfo->localClients.erase(messageEntry.clientToken));
			}
			else
			{
				ClientSessionToken token = NewToken();
				std::shared_ptr<LocalClientSession> client = _hostInfo->localClients[messageEntry.clientToken].lock();
				TE_ASSERT(client != nullptr);
			}

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
	auto it = _hostInfo->localClients.find(message.clientToken);
	if (it == _hostInfo->localClients.end() || it->second.expired()) [[unlikely]]
	{
		return;
	}

	it->second.lock()->Receive(message);
}

void LocalServerSession::AddClient(std::uint32_t clientUID, std::weak_ptr<LocalClientSession> localClient)
{
	ClientSessionToken clientID = ClientUIDToClientID(clientUID);
	if (clientID == 0) [[unlikely]]
	{
		throw std::runtime_error("Invalid client uid");
	}

	_hostInfo->localClients[clientID] = localClient;
}

ClientSessionToken LocalServerSession::NewToken() noexcept
{
	++_latestToken;
	return _latestToken;
}
