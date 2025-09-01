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
#include <variant>
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
      _latestClientSessionID(0)
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

std::size_t LocalServerSession::GetClients() const noexcept
{
	return _hostInfo->localClients.size();
}

void LocalServerSession::Host(const HostArgs &args)
{
	_sessionState = EServerSessionState::Starting;

	_hostInfo = std::make_unique<HostInfo>(args.title, args.password, args.maximumClients);

	EventLocalServerHostData data{
	    .socketType = ESocketType::Local,
	    .serverSlot = _serverSessionSlot,
	};
	GetEventManager().GetCoreEvents().ServerHost().Invoke(&data, {}, EEventInvocation::None);

	_sessionState = EServerSessionState::Hosting;
}

void LocalServerSession::HostAsync(const HostArgs &args, const ServerHostErrorHandler &handler) noexcept
{
	// handler(args, nullptr);
}

void LocalServerSession::Shutdown()
{
	_sessionState = EServerSessionState::Stopping;

	_hostInfo = nullptr;

	TE_TRACE("Shutting down server");

	EventLocalServerShutdownData data{
	    .socketType = ESocketType::Local,
	    .serverSlot = _serverSessionSlot,
	};
	GetEventManager().GetCoreEvents().ServerHost().Invoke(&data, {}, EEventInvocation::None);

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

void LocalServerSession::Disconnect(ClientSessionID clientID, EDisconnectionCode code)
{
	auto it = _hostInfo->localClients.find(clientID);
	if (it == _hostInfo->localClients.end()) [[unlikely]]
	{
		throw std::runtime_error("Invalid local client slot");
	}

	auto &&localClient = it->second.lock();
	if (localClient != nullptr)
	{
		LocalSessionMessage::Disconnect event{
		    .code = code,
		    .clientID = localClient->GetSessionID(),
		};
		_messageQueue.emplace(event, localClient->GetSessionSlot(), _serverSessionSlot);
	}

	_hostInfo->localClients.erase(clientID);
}

void LocalServerSession::SendReliable(ClientSessionID clientSessionID, const NetworkSessionData &data)
{
	Send(clientSessionID, data, ELocalSessionSource::SendReliable);
}

void LocalServerSession::SendUnreliable(ClientSessionID clientSessionID, const NetworkSessionData &data)
{
	Send(clientSessionID, data, ELocalSessionSource::SendUnreliable);
}

void LocalServerSession::Send(ClientSessionID clientSessionID, const NetworkSessionData &data, ELocalSessionSource source)
{
	auto it = _hostInfo->localClients.find(clientSessionID);
	if (it == _hostInfo->localClients.end() || it->second.expired()) [[unlikely]]
	{
		throw std::runtime_error("Client not found");
	}

	it->second.lock()->ReceiveFromServer(LocalSessionMessage{
	    .variant = LocalSessionMessage::Receive{
	        .source = source,
	        .bytes = std::vector<std::byte>(data.bytes.begin(), data.bytes.end()),
	        .clientID = it->second.lock()->GetSessionID(),
	    },
	    .clientSlot = it->second.lock()->GetClientSlot(),
	    .serverSlot = _serverSessionSlot,
	});
}

void LocalServerSession::BroadcastReliable(const NetworkSessionData &data)
{
	Broadcast(data, ELocalSessionSource::BroadcastReliable);
}

void LocalServerSession::BroadcastUnreliable(const NetworkSessionData &data)
{
	Broadcast(data, ELocalSessionSource::BroadcastUnreliable);
}

void LocalServerSession::Broadcast(const NetworkSessionData &data, ELocalSessionSource source)
{
	for (const auto &[clientID, localClient] : _hostInfo->localClients)
	{
		if (!localClient.expired())
		{
			Send(clientID, data, source);
		}
	}
}

void LocalServerSession::EnqueueMessage(ClientSessionID clientSessionID, const NetworkSessionData &data, ELocalSessionSource source)
{
	auto it = _hostInfo->localClients.find(clientSessionID);
	if (it == _hostInfo->localClients.end() || it->second.expired())
	{
		// Client not found or disconnected
		return;
	}

	LocalSessionMessage message{
	    .variant = LocalSessionMessage::Receive{
	        .source = source,
	        .bytes = std::vector<std::byte>(data.bytes.begin(), data.bytes.end()),
	        .clientID = it->second.lock()->GetSessionID(),
	    },
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
		updated = true;

		LocalSessionMessage &messageEntry = _messageQueue.front();

		if (std::holds_alternative<LocalSessionMessage::Connect>(messageEntry.variant))
		{
			auto &event = std::get<LocalSessionMessage::Connect>(messageEntry.variant);

			if (!event.localClient.expired())
			{
				++_latestClientSessionID;
				ClientSessionID clientID = _latestClientSessionID;

				TE_TRACE("Connect event, client", clientID);

				EventLocalServerConnectData data{
				    .socketType = ESocketType::Local,
				    .clientID = clientID,
				    .clientSlot = messageEntry.clientSlot,
				    .serverSlot = messageEntry.serverSlot,
				};

				coreEvents.ServerConnect().Invoke(&data, data.socketType, EEventInvocation::None);

				std::shared_ptr<LocalClientSession> client = event.localClient.lock();
				_hostInfo->localClients[clientID] = client;

				// Send connected client's session id
				BroadcastReliable(NetworkSessionData{
				    .bytes = std::vector<std::byte>{
				        std::byte('\0'),
				        std::byte(clientID),
				    },
				    .channelID = 0,
				});
			}
		}
		else if (std::holds_alternative<LocalSessionMessage::Disconnect>(messageEntry.variant))
		{
			auto &event = std::get<LocalSessionMessage::Disconnect>(messageEntry.variant);

			EventLocalServerDisconnectData data{
			    .socketType = ESocketType::Local,
			    .code = event.code,
			    .clientSlot = messageEntry.clientSlot,
			    .serverSlot = messageEntry.serverSlot,
			};

			coreEvents.ServerDisconnect().Invoke(&data, data.socketType, EEventInvocation::None);
		}
		else if (std::holds_alternative<LocalSessionMessage::Receive>(messageEntry.variant))
		{
			auto &event = std::get<LocalSessionMessage::Receive>(messageEntry.variant);

			EventLocalServerMessageData eventData{
			    .socketType = ESocketType::Local,
			    .clientID = event.clientID,
			    .message = std::string_view(reinterpret_cast<const char *>(event.bytes.data()), event.bytes.size()),
			    .broadcast = "",
			    .clientSlot = messageEntry.clientSlot,
			    .serverSlot = messageEntry.serverSlot,
			};

			GetEventManager().GetCoreEvents().ServerMessage().Invoke(&eventData, _serverSessionSlot, EEventInvocation::None);

			if (eventData.broadcast != "")
			{
				NetworkSessionData data{
				    .bytes = std::span<const std::byte>(reinterpret_cast<const std::byte *>(eventData.message.data()), eventData.message.size()),
				    .channelID = 0,
				};
				BroadcastReliable(data);
			}
		}
		else [[unlikely]]
		{
		}

		_messageQueue.pop();
	}

	return updated;
}

void LocalServerSession::ReceiveFromClient(const LocalSessionMessage &message) noexcept
{
	_messageQueue.emplace(message);
}

void LocalServerSession::ConnectClient(NetworkSessionSlot clientSlot, std::weak_ptr<LocalClientSession> localClient)
{
	_messageQueue.emplace(LocalSessionMessage::Connect(localClient), clientSlot, _serverSessionSlot);
}

ClientSessionID LocalServerSession::NewID() noexcept
{
	++_latestClientSessionID;
	return _latestClientSessionID;
}
