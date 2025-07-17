#include "LocalServer.hpp"

#include "LocalClient.hpp"
#include "SocketType.hpp"

#include <optional>

using namespace tudov;

LocalServer::Message::Message(bool reliable, std::string data) noexcept
{
	this->source = reliable ? BroadcastReliable : BroadcastUnreliable;
	this->data = data;
}

LocalServer::Message::Message(bool reliable, ClientID clientID, std::string data) noexcept
{
	this->source = reliable ? SendReliable : SendUnreliable;
	this->clientID = clientID;
	this->data = data;
}

LocalServer::LocalServer(INetwork &network) noexcept
    : _network(network)
{
}

INetwork &LocalServer::GetNetwork() noexcept
{
	return _network;
}

ESocketType LocalServer::GetSocketType() const noexcept
{
	return ESocketType::Local;
}

bool LocalServer::IsHosting() noexcept
{
	return _hostInfo != nullptr;
}

void LocalServer::Host(const HostArgs &args)
{
	_hostInfo = std::make_unique<HostInfo>(HostInfo{
	    .title = args.title,
	    .password = args.password,
	    .maximumClients = args.maximumClients,
	});
}

void LocalServer::Shutdown()
{
	_hostInfo = nullptr;
}

std::optional<std::string_view> LocalServer::GetTitle() noexcept
{
	return _hostInfo != nullptr ? std::make_optional(_hostInfo->title) : std::nullopt;
}

std::optional<std::string_view> LocalServer::GetPassword() noexcept
{
	return _hostInfo != nullptr ? std::make_optional(_hostInfo->password) : std::nullopt;
}

std::optional<std::size_t> LocalServer::GetMaxClients() noexcept
{
	return _hostInfo != nullptr ? std::make_optional(_hostInfo->maximumClients) : std::nullopt;
}

void LocalServer::SendReliable(ClientID clientID, std::string_view data)
{
	_messageQueue.emplace(true, clientID, std::string(data));
}

void LocalServer::SendUnreliable(ClientID clientID, std::string_view data)
{
	_messageQueue.emplace(false, clientID, std::string(data));
}

void LocalServer::BroadcastReliable(std::string_view data)
{
	_messageQueue.emplace(true, std::string(data));
}

void LocalServer::BroadcastUnreliable(std::string_view data)
{
	_messageQueue.emplace(false, std::string(data));
}

bool LocalServer::Update()
{
	bool updated = false;

	std::unordered_map<ClientID, std::weak_ptr<LocalClient>> &localClients = _hostInfo->localClients;
	for (auto &&it = localClients.begin(); it != localClients.end();)
	{
		if (it->second.expired())
		{
			it = localClients.erase(it);
		}
		else
		{
			++it;
		}
	}

	while (!_messageQueue.empty())
	{
		Message &message = _messageQueue.front();

		switch (message.source)
		{
		case Message::SendReliable:
		case Message::SendUnreliable:
			if (auto &&it = localClients.find(message.clientID); it != localClients.end())
			{
				it->second.lock()->Receive(message.data);
				updated = true;
			}
		case Message::BroadcastReliable:
		case Message::BroadcastUnreliable:
			for (auto &&[clientID, localClient] : localClients)
			{
				localClient.lock()->Receive(message.data);
				updated = true;
			}
			break;
		[[unlikely]] default:
			break;
		}

		_messageQueue.pop();
	}

	return updated;
}

void LocalServer::AddClient(std::uint32_t uid, const std::weak_ptr<LocalClient> &localClient)
{

	// TODO
}
